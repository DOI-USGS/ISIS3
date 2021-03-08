#ifndef Database_h
#define Database_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <iostream>
#include "DbProfile.h"
#include "IException.h"
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
   *          QString/StringTools merge
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
      Database(const QString &connName, const QString &driverType);
      Database(const QString &name, Access dbConn = Connect);
      Database(const DbProfile &profile, Access dbConn = Connect);
      virtual ~Database();

      /**
       * @brief Return the name of this database as specifed upon creation
       *
       * @return QString The name of this database
       */
      QString Name() const {
        return (_name);
      }

      void makePersistant();
      bool isPersistant() const;
      void setAsDefault();
      Database clone(const QString &name) const;
      QStringList getTables() const;
      QStringList getViews() const;
      QStringList getSystemTables() const;

      static void remove(const QString &name);

      static bool addAccessConfig(const QString &confFile);
      static DbProfile getProfile(const QString &name);

    protected:
      Database(const QSqlDatabase &other, const QString &name);
      QSqlDatabase init(const DbProfile &profile, Access dbConn = Connect);
      QSqlDatabase init(const QString &name = "",
                        const QString &driverType = "");
      void configureAccess(QSqlDatabase &db, const DbProfile &profile);

    private:
      static QString _actualConnectionName; /** Needed due to peculiar
                                                 * issues with Database
                                                 * construction techniques */
      QString _name;           //!<  Name of the connection

      void tossDbError(const QString &message, const char *f, int l) const;
  };

}
#endif
