/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/05/02 18:39:28 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <iostream>
#include <algorithm>

#include <QDir>

#include "SpiceDbGen.h"
#include "NaifStatus.h"

using namespace std;
using namespace Isis;


const char *SpiceDbGen::calForm = "YYYY MON DD HR:MN:SC.###### TDB ::TDB";
/**
 * Constructs a new SpiceDbGen.
 *
 * @param type The type of kernel to be processed. Either, "SPK" or "CK.
*/
SpiceDbGen::SpiceDbGen(QString type) {
  p_type = type;
//  calForm = "YYYY MON DD HR:MN:SC.### TDB ::TDB";
}


/**
 * Creates a Pvl object that stores all of the kernels under the condition
 * specified by the filter.
 *
 * @param quality   The quality of the kernels that are being filtered into
 *                  the database. For example, "Reconstructed".
 *
 * @param location  The directory in which the method searches for kernels.
 *
 * @param filter    Vector of QStrings containing regular expression used to
 *                  match kernels of a particular quality. This parameter is
 *                  used to distinguish between kernels of different qualities
 *                  and/or different missions that may be placed in the same
 *                  directory.
 *
 * @return PvlObject
 *
 * @internal
 *   @history 2010-04-27 Stuart Sides Changed to work with vector of filters
 *   instead of a single filter
 *
 *   @history 2015-03-27 Stuart Sides Changed the number of decimal places
 *   written to the Time keyword for kernel.###.db files from three to six
 *
 * @throws Isis::iException::Message
*/
PvlObject SpiceDbGen::Direct(QString quality, QString location,
                             std::vector<QString> &filter) {
  PvlObject result;
  QString type = "none";

  for (unsigned int i = 0; i < filter.size(); ++i) {
    //Create a list of all of the files matching the current filter
    QStringList files = GetFiles(FileName(location), filter[i]);
    PvlObject pvlKernel("change");

    // Throw an error if no files are being added to this database for
    // this filter/regex
    if (files.size() == 0) {
      QString message = "Your filter [" + location + "/" + filter[i] + "]"
                       + "has not detected any " + quality + " kernels";
      throw IException(IException::User, message, _FILEINFO_);
    }

    for (int fileNum = 0 ; fileNum < files.size() ; fileNum++) {
      FileName currFile((QString) location + "/" + files[fileNum]);
      PvlGroup selection = AddSelection(currFile);
      selection += PvlKeyword("Type", quality);
      result.addGroup(selection);
    }
  }

  // Check each group to make sure it is the same type as others
  PvlObject::PvlGroupIterator grp = result.beginGroup();

  while (grp != result.endGroup()) {
    // The kernel did not have any time coverage, so ignore it
    if (grp->name() == "No coverage" || grp->name() == "Null") {
      result.deleteGroup(grp->name());
    }
    // This is used for the first time thru the while loop
    // DO NOT increment grp here
    else if (type == "none") {
      type = grp->name();
    }
    else if (grp->name() == type) {
      grp->setName("Selection");
      grp++;
    }
    else {
      QString message = "A kernel of type [" + grp->name() + "] has been found in a directory for type [" + type + "]" ;
      throw IException(IException::Programmer, message, _FILEINFO_);
      break;
    }
  }

  if (type == "SPK") {
    result.setName("SpacecraftPosition");
  }
  else if (type == "CK") {
    result.setName("SpacecraftPointing");
  }

  return result;
}

/**
  * Essenetially a method call to the underlying QDir class which will filter
  * the files needed by Direct(). Files are returned in order of the time they
  * were most recently modified, the oldest file being first and the most
  * recently modified being last.
  *
  * @param location  The directory in which the method searches for files.
  *
  * @param filter    The regular expression used to match files of a
  *                  particular quality. This parameter is important, since it
  *                  is necessary to distinguish between kernels of different
  *                  qualities and/or different missions that may be placed in
  *                  the same directory.
  *
  * @return QStringList
  *
  */
QStringList SpiceDbGen::GetFiles(FileName location, QString filter) {
  filter.remove("\\");
  QDir dir(location.expanded(), filter,
           QDir::Name, QDir::Files);
  return dir.entryList();
}


/**
  * Format a single kernel file to include the file.
  *
  * @param fileIn   The file name being added
  *
  * @return PvlGroup
  *
  * @throws Isis::iException::Message
  */
PvlGroup SpiceDbGen::AddSelection(FileName fileIn) {
  NaifStatus::CheckErrors();

  //finalize the filename so that it may be used in spice routines
  QString tmp = fileIn.expanded();
//  const char* file = fileIn.expanded().c_str();
  furnsh_c(tmp.toLatin1().data());
  SpiceChar fileType[32], source[2048];
  SpiceInt handle;

  SpiceBoolean found;
  kinfo_c(tmp.toLatin1().data(), 32, 2048, fileType, source, &handle, &found);
  QString currFile = fileType;

  //create a spice cell capable of containing all the objects in the kernel.
  SPICEINT_CELL(currCell, 1000);
  //this resizing is done because otherwise a spice cell will append new data
  //to the last "currCell"
  ssize_c(0, &currCell);
  ssize_c(1000, &currCell);

  //select which spice coverage routine to use. If a text kernel is detected, it
  //will be returned here and weeded out at the end of Direct(). This helps
  //to protect the user from inadvertently adding "." and ".." to their filters
  if (currFile == "SPK") {
    spkobj_c(tmp.toLatin1().data(), &currCell);
  }
  else if (currFile == "CK") {
    ckobj_c(tmp.toLatin1().data(), &currCell);
  }
  else if (currFile == "TEXT") {
    return PvlGroup("No coverage");
  }

  PvlGroup result;
  //iterate through every body in the kernel
  for(int bodyCount = 0 ; bodyCount < card_c(&currCell) ; bodyCount++) {
    //get the NAIF body code
    int body = SPICE_CELL_ELEM_I(&currCell, bodyCount);

    //only provide coverage for negative NAIF codes
    //(Positive codes indicate planetary bodies, negatives indicate
    // spacecraft and instruments)
    if (body < 0) {
      NaifStatus::CheckErrors();

      //find the correct coverage window
      if (currFile == "SPK") {
        //  2000 is the max coverage window size for an SPK kernel
        SPICEDOUBLE_CELL(cover, 2000);
        ssize_c(0, &cover);
        ssize_c(2000, &cover);
        spkcov_c(tmp.toLatin1().data(), body, &cover);

        NaifStatus::CheckErrors();

        result = FormatIntervals(cover, currFile);
      }
      else if (currFile == "CK") {
        //  200,000 is the max coverage window size for a CK kernel
        SPICEDOUBLE_CELL(cover, 200000);
        ssize_c(0, &cover);
        ssize_c(200000, &cover);
        ckcov_c(tmp.toLatin1().data(), body, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);

        NaifStatus::CheckErrors();

        result = FormatIntervals(cover, currFile);
      }
    }
  }

  QString outFile = fileIn.originalPath();
  result += PvlKeyword("File", outFile + "/" + fileIn.name());

  NaifStatus::CheckErrors();

  // Unfurnishes tmp file to prevent file table overflow
  unload_c(tmp.toLatin1().data());

  return result;
}


PvlGroup SpiceDbGen::FormatIntervals(SpiceCell &coverage, QString type) {
  NaifStatus::CheckErrors();

  PvlGroup result(type);
  SpiceChar begStr[35], endStr[35];
  //Get the number of intervals in the object.
  int niv = card_c(&coverage) / 2;
  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  for(int j = 0;  j < niv;  j++) {
    //Get the endpoints of the jth interval.
    wnfetd_c(&coverage, j, &begin, &end);
    //Convert the endpoints to TDB calendar
    timout_c(begin, calForm, 35, begStr);
    timout_c(end, calForm, 35, endStr);
    result += PvlKeyword("Time", "(\"" + (QString)begStr +
                         "\", \"" + (QString)endStr + "\")");
  }

  NaifStatus::CheckErrors();

  return result;
}


void SpiceDbGen::FurnishDependencies(QList<FileName> sclks, QList<FileName> lsks,
                                     QList<FileName> extras) {
  NaifStatus::CheckErrors();

  // furnish the lsk files
  foreach (FileName lsk, lsks) {
    furnsh_c(lsk.expanded().toLatin1().data());
  }

  // furnish the sclk files
  foreach (FileName sclk, sclks) {
    furnsh_c(sclk.expanded().toLatin1().data());
  }

  // furnish the extra files
  foreach (FileName extra, extras) {
    furnsh_c(extra.expanded().toLatin1().data());
  }

  NaifStatus::CheckErrors();
}
