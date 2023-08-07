#ifndef IsisDataFixtures_h
#define IsisDataFixtures_h

#include <map>
#include <string>

#include <QString>
#include <QMap>

#include "FileName.h"
#include "TempFixtures.h"

#include <nlohmann/json.hpp>
using json_t = nlohmann::ordered_json;


namespace Isis {
/**
 * @brief Provides a simulated ISISDATA directory tree
 *
 */
  class IsisDataInventory : public TempTestingFiles {
    private:
      class IsisDataInventoryFile {
        public:
          IsisDataInventoryFile( const QString &fname, const QFileInfo &finfo,
                            const json_t &jsondata = json_t::object() ) {
            m_filename = fname;
            m_fileinfo = finfo;
            m_jsondata = jsondata;
          }
          ~IsisDataInventoryFile( ) { }

          inline const QString &name() const {
            return ( m_filename );
          }

          inline const QFileInfo &info() const {
            return ( m_fileinfo );
          }

          inline const json_t &data() const {
            return ( m_jsondata );
          }

          inline bool contains( const QString &key ) const {
            return ( m_jsondata.contains( key.toStdString() ) );
          }

          inline size_t size() const {
            return ( info().size() );
          }

          inline bool compare( const IsisDataInventoryFile &q1,
                               const IsisDataInventoryFile &q2 ) const {
            return ( q1.m_filename  < q2.m_filename );
          }

        protected:
          QString    m_filename;
          QFileInfo  m_fileinfo;
          json_t     m_jsondata;
      };

    protected:
      typedef QMap<QString, IsisDataInventoryFile>  IsisDataInventoryMap;
      typedef QMap<QString, IsisDataInventoryFile>  IsisDataDirectoryMap;

      QString system_isisdata() const;
      QString isisdatadir() const;
      QString isisdata_path() const;

      inline const IsisDataInventoryMap &inventory() const {
        return ( m_isisdata_inventory );
      }

      inline const IsisDataDirectoryMap &directories() const {
        return ( m_isisdata_directories );
      }

      size_t  size() const;

      QString scrub_path_prefix( const QString &fname,
                                 const QString &path_prefix = ".") const;

      void SetUp() override;
      void TearDown() override;

    private:
      // Data from ISISDATA simulated file setup
      FileName              m_system_isisdata;
      FileName              m_isisdatadir;
      IsisDataInventoryMap  m_isisdata_inventory;
      IsisDataDirectoryMap  m_isisdata_directories;

      json_t get_real_file_info( const QString &fname ) const;

  };

} // namespace Isis

#endif