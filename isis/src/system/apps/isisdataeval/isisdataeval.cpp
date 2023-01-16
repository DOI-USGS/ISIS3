/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "isisdataeval.h"

#include <array>
#include <vector>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include <QString>
#include <QStringList>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QCryptographicHash>

#include "Application.h"
#include "FileName.h"
#include "IException.h"
#include "IsisDataModel.h"
#include "IString.h"
#include "iTime.h"
#include "Preference.h"
#include "Process.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "UserInterface.h"


namespace Isis {

  // Might be able to use brackets here but the main may not be callable
  // because its not in the Isis namespace. This should suffice.
  using namespace Data;

  //********************************************************************
  // Helper functions
  //********************************************************************

  /** Add log Group to log file and console for backward compatability */
  inline void db_addLogGroup( Pvl *log, PvlGroup &group ) {
    // Report translations...
    // Emulates: pvl->addLogGroup( group );
    log->addGroup( group );
    Application::Log( group );
    return;
  }

  /** Report evaluation data consistently */
  inline void report_issues( std::ostream &db_os, const Data::DBFileDispositionList &db_status,
                             int &v_missing, int &v_empty, int &v_symlinks, int &v_externals ) {

    if ( db_status.size() > 0) {
      db_os << "status, filespec, sourcespec, source, target, category" << std::endl;

      for ( auto const &db_file : db_status ) {
        QString v_status = db_file.name().toLower();
        db_os << v_status << ","
              << db_file.key()  << ","
              << db_file.datum().name() << ","
              << db_file.datum().expanded() << ","
              << db_file.datum().target() << ","
              << db_file.status()
              << std::endl;

        if ( "missing"  == v_status ) v_missing++;
        if ( "empty"    == v_status ) v_empty++;
        if ( "symlink"  == v_status ) v_symlinks++;
        if ( "external" == v_status ) v_externals++;
      }
    }
  }

//*******************************************************************
// isisdataeval main
//*******************************************************************
  void isisdataeval( UserInterface &ui, Pvl *log ) {

    Process eval_proc;

    // Load any preferences file if requested. Note this is the same as adding
    // adding a "-pref=PREFERENCES" except this logs the preferences file used.
    if ( ui.WasEntered( "PREFERENCES" ) ) {
      Preference::Preferences().Load( ui.GetAsString( "PREFERENCES" ) );
    }

    // Get a reference to the DataDirectory group for translations
    PvlGroup &prefdir = Preference::Preferences().findGroup("DataDirectory");

    // Determine the DATADIR to evaluate
    QString datadir = ui.GetString( "DATADIR" );
    FileName file_datadir( datadir );
    DBFileStatus f_info( datadir );
    QString dataroot = datadir;

    std::cout << std::endl;
    std::cout << "DATAROOT = " << f_info.original() << std::endl;
    std::cout << "DATAROOT = " << f_info.expanded() << std::endl;

    if ( !f_info.isDirectory() ) {
        throw IException( IException::User,
                        "DATADIR (" + datadir + ") is not a directory!",
                        _FILEINFO_ );
    }

    // Get the DataDirectory from the Preferences file
    FileName isisdata( "$ISISDATA" );
    std::cout << std::endl;
    std::cout << "ISISDATA = " << isisdata.expanded() << std::endl;

    // Now reset ISISDATA if requested by user
    if ( ui.WasEntered( "ISISDATA" ) ) {
      isisdata = ui.GetAsString("ISISDATA");
      PvlKeyword iroot( "ISISDATA", isisdata.expanded() );
      prefdir.addKeyword( iroot, PvlContainer::Replace );
      std::cout << "ISISDATA = " << isisdata.expanded() << std::endl;
      std::cout << "ISISDATA reset by user!" << std::endl;
      std::cout << std::endl;
    }

    // Report translations...
    // pvl->addLogGroup( prefdir );
    db_addLogGroup( log, prefdir );

    //*******************************************************************
    // Process DATADIR which will collect the inventory and evaluate
    // the kernel kernel_????.db and kernel_????.conf
    // Traverse DATADIR using ISISDATA as $ISISDATA volume translations.
    //*******************************************************************
    IsisDataModel v_isisdatadir( datadir, isisdata.expanded() );

    // Run the evaluation of the kernel db/conf configuration
    BigInt t_install_size = v_isisdatadir.evaluate();
    double t_volume_size = (double) t_install_size / (1024.0 * 1024.0 * 1024.0);

    // Collect evaluation data
    size_t s_dirs;  // total directories in dataroot
    BigInt t_allfiles = v_isisdatadir.allFilesCount( &s_dirs );
    BigInt t_dirs = s_dirs;
    BigInt t_files = t_allfiles - t_dirs;

    // The counts for *.db and *.conf files found
    BigInt t_kerneldbs = v_isisdatadir.dbCount();
    BigInt t_configs   = v_isisdatadir.configCount();

    // Problem areas
    DBFileDispositionList kernel_status;
    int v_bad = v_isisdatadir.validate( kernel_status );
    std::cout << "\nValidation Complete..." << v_bad << " issues found!" << std::endl;

    // Report kernel validation status to console
    int v_missing(0), v_empty(0), v_symlinks(0), v_externals(0);
    report_issues(std::cout, kernel_status, v_missing, v_empty, v_symlinks, v_externals );

    // Generate the result log
    std::cout << std::endl;
    PvlGroup results("Results");
    results.addKeyword( PvlKeyword( "ISISDATA", isisdata.expanded() ) );
    results.addKeyword( PvlKeyword( "DATADIR", datadir ) );
    results.addKeyword( PvlKeyword( "EmptyKernelDBs", toString( v_empty ) ) );
    results.addKeyword( PvlKeyword( "MissingKernelDBs", toString( v_missing ) ) );
    results.addKeyword( PvlKeyword( "SymlinkKernelFiles", toString( v_symlinks ) ) );
    results.addKeyword( PvlKeyword( "ExternalKernelFiles", toString( v_externals ) ) );
    results.addKeyword( PvlKeyword( "TotalDBConfigFiles", toString( t_configs ), "conf" ) );
    results.addKeyword( PvlKeyword( "TotalKernelDBFiles", toString( t_kerneldbs ), "db" ) );
    results.addKeyword( PvlKeyword( "TotalDirectories", toString( t_dirs ) ) );
    results.addKeyword( PvlKeyword( "TotalDataFiles", toString( t_files ) ) );
    results.addKeyword( PvlKeyword( "TotalInstallSize", toString( t_install_size ), "bytes" ) );
    results.addKeyword( PvlKeyword( "TotalVolumeSize", toString( t_volume_size ), "GB" ) );

    // If users wants kernel issues reported, write it out here
    if ( ui.WasEntered( "TOISSUES" ) ) {
      FileName toissues = ui.GetFileName( "TOISSUES" );

      // Only write the file if there are missing files
      if ( kernel_status.size() > 0 ) {
        std::ofstream os;
        os.open( toissues.expanded().toLatin1().data(), std::ios::out );
        if (!os ) {
          QString mess = "Unable to open/create " + toissues.expanded();
          throw IException( IException::User, mess, _FILEINFO_ );
        }

        // Write the results
        v_missing = v_empty = v_symlinks = v_externals = 0;
        report_issues( os, kernel_status, v_missing, v_empty, v_symlinks, v_externals );

        // All done...
        os.close();
      }
    }

    //*******************************************************************
    // Process all the data found in DATADIR. If DATADIR = ISISDATA,
    // the complete ISISDATA install is validated.
    //*******************************************************************
    // If user wants to validate the whole of DATADIR, this is it.
    const bool needInventory = ui.WasEntered( "TOINVENTORY" );

    // Set up default hash and determine if requested by user
    QCryptographicHash::Algorithm hash_algorithm = QCryptographicHash::Md5;
    QString  hashtype = ui.GetString( "HASH" ).toLower();
    const bool needHash = ( "nohash" != hashtype );

    // Either case will kick off the inventory.
    if ( needInventory || needHash ) {

      // Check if user wants detailed log of DATADIR
      QString inventory_file( "/dev/null" );
      if ( needInventory ) {
        FileName toinventory = ui.GetFileName( "TOINVENTORY" );
        inventory_file = toinventory.expanded();
      }

      if ( needHash ) {
        // Get the algorithm of choice
        if ( "md5"    == hashtype ) hash_algorithm = QCryptographicHash::Md5;
        if ( "sha1"   == hashtype ) hash_algorithm = QCryptographicHash::Sha1;
        if ( "sha256" == hashtype ) hash_algorithm = QCryptographicHash::Sha256;
      }

      // Only write the file if there are missing files
      if ( v_isisdatadir.size() > 0 ) {

        std::ofstream os;
        os.open( inventory_file.toLatin1().data(), std::ios::out );
        if (!os ) {
          QString mess = "Unable to open/create " + inventory_file;
          throw IException( IException::User, mess, _FILEINFO_ );
        }

        // Create the header output from the first file in the inventory.
        // Note its assured to exist.  Add the hash field if requested.
        QStringList header = v_isisdatadir.allfiles().cbegin()->header();

        // Set the hashtag
        QString hashtag( hashtype );
        if ( needHash ) {
          hashtag = hashtype + "hash";
          header.append( hashtag );
        }

        os << header.join(",") << std::endl;

        std::cout << "Running inventory ..." << std::endl;
        Progress v_progress;
        v_progress.SetText("inventory+"+hashtag);
        v_progress.SetMaximumSteps( v_isisdatadir.size() );
        v_progress.CheckStatus();
        BigInt n_symlinks = 0;
        QCryptographicHash volume_hash( hash_algorithm );

        // Determine size (MB) of file buffer for hashing only if requested
        std::unique_ptr<char[]>  file_data;
        qint64 HashBufferSizeBytes = 1024 * 1024 * 256;  // Default size
        if ( needHash ) {
          HashBufferSizeBytes = 1024 * 1024 * ui.GetInteger("HASHBUFFER");
          // Consistent with the Qt 5.15 API
          file_data.reset( new char[HashBufferSizeBytes] );
        }

        DBFileDispositionList inventory_errors;
        const qint64 MaxBytesToRead = HashBufferSizeBytes;

        for ( auto const &dbfile : v_isisdatadir.allfiles()  ) {

          if ( !dbfile.isDirectory() ) {

            // Check for symbolic links
            if ( dbfile.isSymbolicLink() ) {
              n_symlinks++;

              QString symtarget = dbfile.info().symLinkTarget();
              DBFileStatus symfile( symtarget );

              // Report symlink
              inventory_errors.push_back( DBFileDisposition( "Symlink", dbfile.name(), symfile, "inventory" ) );

              if ( !symfile.exists() ) {
                inventory_errors.push_back( DBFileDisposition( "Missing", dbfile.info().symLinkTarget(), dbfile, "nosymlink" ) );
              }
              else {
                if ( !v_isisdatadir.allfiles().contains( symfile.original() ) ) {
                  inventory_errors.push_back( DBFileDisposition( "External", symfile.name(), dbfile, "symlink" ) );
                }
              }
            }
            else {

              // Create the values array from json object
              os << dbfile.values().join(",");

              // If hashing has been requested, do it here. We are computing two
              // hashes - one is individual file hash, the other is the complete
              // volume hash.
              if ( needHash ) {
                QCryptographicHash file_hash( hash_algorithm );

                // File exists, lets open it and compute the hash
                QFile v_file( dbfile.expanded() );
                if ( !v_file.open( QIODevice::ReadOnly ) ) {
                  inventory_errors.push_back( DBFileDisposition( "error", dbfile.expanded(), dbfile, "openfailed" ) );
                  os << "," << db_null();
                }
                else {
                  // Read (in 1 MB chunks) bytes and add to hahses
                  while ( !v_file.atEnd() ) {
                    qint64 nread = v_file.read(file_data.get(), MaxBytesToRead );

                    // Add to hashes
                    file_hash.addData(   file_data.get(), nread );
                    volume_hash.addData( file_data.get(), nread );
                  }

                  os << "," << QString::fromUtf8( file_hash.result().toHex() );
                }
              }

              // Terminate the line and on to the next one
              os << std::endl;
            }
          }

          v_progress.CheckStatus();
        }

        // Report any issues found with inventory...
        std::cout << "\nInventory Complete..." << inventory_errors.size() << " issues found!" << std::endl;
        if ( inventory_errors.size() > 0) {
          v_missing = v_empty = v_symlinks = v_externals = 0;
          report_issues( std::cout, inventory_errors, v_missing, v_empty, v_symlinks, v_externals );

          // If users wants the missing reported, write it out here
          if ( ui.WasEntered( "TOERRORS" ) ) {

            FileName toerrors = ui.GetFileName( "TOERRORS" );

            // Only write the file if there are missing files
            std::ofstream error_os;
            error_os.open( toerrors.expanded().toLatin1().data(), std::ios::out );
            if (!error_os ) {
              QString mess = "Unable to open/create " + toerrors.expanded();
              throw IException( IException::User, mess, _FILEINFO_ );
            }

            // Write the results
            v_missing = v_empty = v_symlinks = v_externals = 0;
            report_issues( error_os, inventory_errors, v_missing, v_empty, v_symlinks, v_externals );

            // All done...
            error_os.close();
          }

        }

        // Log inventory sym links found
        results.addKeyword( PvlKeyword( "InventorySymLinks", toString( n_symlinks ) ) );
        if ( needHash ) {
          QByteArray v_hash_data = volume_hash.result();
          QString volume_hash_str = QString::fromUtf8( v_hash_data.toHex() );
          BigInt hbsize = HashBufferSizeBytes;

          results.addKeyword( PvlKeyword( "HashBufferSize", toString(hbsize), "bytes" ) );
          results.addKeyword( PvlKeyword( "TotalVolumeHash", volume_hash_str, hashtype ) );
        }

        // All done...
        os.close();
      }
    }

    // Final log
    // pvl->addLogGroup( results );
    db_addLogGroup( log, results );

    eval_proc.Finalize();
    return;
  }

} // namespace Isis
