#if !defined(Database_h)
#define Database_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $
 * $Date: 2008/10/30 16:38:23 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include <string>
#include <vector>
#include <iostream>
#include "DbProfile.h"
#include "iException.h"
#include <QSqlDatabase>

namespace Isis {

/**                                          
 * @brief Isis database class providing generalized access to a variety of
 *        databases
 * 
 * This class provides database connections within the Isis application
 * programming interface (API) environment.  It is based upon the
 * DatabaseFactory class and utilizes its features to let users control access
 * to databases.  See the documentation for that class to get a full description
 * of generalized access methods as defined by users.
 * 
 * This class also provides programmer derived access using either database
 * profiles (DbProfile) containing access (DbAccess) specifications.
 * @code
 * //  Use the UPC profile
 *  Database upc("upc")
 * @endcode
 * 
 * Connections can be made to specific databases using named drivers also
 * provided from the DatabaseFactory class.
 * @code
 * //  Set up one for UPC directly
 *  Database upc1("upcDirect", "PostgreSQL");   // "QPSQL" also works
 *  upc1.setHostName("upcdb0.wr.usgs.gov");
 *  upc1.setDatabaseName("upc");
 *  upc1.setUserName("upcread");
 *  upc1.setPort(3309);
 *  upc1.open();
 * @endcode
 * 
 * Since this class is derived from the Qt QSqlDatabase class, it can and is
 * intended to be used in the Qt environment directly.  IMPORTANT NOTE:  The
 * init() function returns a QSqlDatabase instance in all constructors which
 * means that the class has not yet completed constructing.  The implications of
 * this are that the Database class elements, namely data constructs, are
 * generally off limits until after the return from init().  This is primarily
 * the reason for some of the implimentation decisions made in this class.
 * 
 * See also SqlQuery and SqlRecord.
 * 
 * @ingroup Database
 * @author 2006-08-18 Kris Becker
 *  
 * @internal 
 *   @history 2007-06-05 Brendan George - Modified to work with
 *          iString/StringTools merge
 *   @history 2008-10-30 Steven Lambright - tossDbError now accepts a const
 *            char* for a filename, issue pointed out by "novus0x2a" (Support
 *            Board Member)
 */
  class Database : public QSqlDatabase {
    public:
      /** Access status for database creation */
      typedef  enum { 
        Connect,      //!< Connect to database immediately
        DoNotConnect  //!< Do not connect to database
      } Access;

      Database();
      Database(Access dbConn);
      Database(const std::string &connName, const std::string &driverType);
      Database(const std::string &name, Access dbConn = Connect);
      Database(const DbProfile &profile, Access dbConn = Connect);
      virtual ~Database();

      /**
       * @brief Return the name of this database as specifed upon creation
       * 
       * @return std::string The name of this database
       */
      std::string Name() const { return (_name);  }

      void makePersistant();
      bool isPersistant() const;
      void setAsDefault();
      Database clone(const std::string &name) const;
      std::vector<std::string> getTables() const;
      std::vector<std::string> getViews() const;
      std::vector<std::string> getSystemTables() const;

      static void remove(const std::string &name);

      static bool addAccessConfig(const std::string &confFile);
      static DbProfile getProfile(const std::string &name);

    protected:
      Database(const QSqlDatabase &other, const std::string &name);
      QSqlDatabase init(const DbProfile &profile, Access dbConn = Connect);
      QSqlDatabase init(const std::string &name = "", 
                        const std::string &driverType = "");
      void configureAccess(QSqlDatabase &db, const DbProfile &profile);

    private:
      static std::string _actualConnectionName; /** Needed due to peculiar
                                                 * issues with Database 
                                                 * construction techniques */
      std::string _name;           //!<  Name of the connection

      void tossDbError(const std::string &message, const char *f, int l) const
                        throw (iException &);
  };

}
#endif
