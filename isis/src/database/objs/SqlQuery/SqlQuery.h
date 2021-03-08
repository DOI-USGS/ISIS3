#ifndef SqlQuery_h
#define SqlQuery_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <iostream>
#include "IException.h"
#include "Database.h"
#include <QSqlQuery>
#include <QSqlError>

namespace Isis {

  class SqlRecord;

  /**
   * @brief Construct and execute a query on a database and manage result
   *
   * This class is derived from the
   * <a href="http://doc.trolltech.com/4.1/qsqlquery.html">Qt QSqlQuery</a> class.
   * It is designed to be used in conjunction with the functionality of that
   * class.
   *
   * The major features are to to make it easier to specify what happens when
   * errors occurs (setThrowOnFailure()) and return and accept query strings and
   * results using the standard C++ library and classes (vector, string).  It
   * exists mainly as a convenience class interface to the Qt QSqlQuery class,
   * providing standard C++ elements as opposed to Qt elements.
   *
   * \b NOTE all constructors initially set exception throwing as the default.
   *
   * Some examples follow:
   *
   * This example uses some of Qt's features, which you can use at will throughout
   * this and other classes.  This one uses the named/positional binding features"
   * @code
   * Database mptdb("matchpoints");
   *
   *  //  We now have a match point table.  Populate it with the data read in.
   *  SqlQuery inserter(mptdb);
   *  inserter.setThrowOnFailure();
   *  inserter.prepare("INSERT INTO "
   *                   "matchpt (pointid, fsc, line, samp, class, diameter) "
   *                   "VALUES (:pointid, :fsc, :line, :samp, :class,:diameter)");
   *  mptdb.transaction();
   *  for (int row = 0 ; row < mpt.rows() ; row++) {
   *    CSVReader::CSVAxis matchpt = mpt.getRow(row);
   *    for (int i = 0 ; i < 6 ; i++) {*
   *      inserter.bindValue(i, QVariant(matchpt[i].c_str()));
   *    }
   *    cout << "Inserting row " << row << ", status..." << inserter.exec()
   *         << endl;
   *
   *  }
   *  mptdb.commit();
   *  mptdb.close();
   *}
   * @endcode
   *
   * Thats all well and good but Qt's binding is restricted to using it in the
   * INSERT/VALUES constructs.  They have no support for binding used outside this
   * SQL operation.  Below is an example with a more general approach provided
   * with this API, although a bit less robust.
   *
   * @code
   *  auto_ptr<Database> db = auto_ptr<Database> (new Database(dbProf,
   *                                                           Database::Connect));
   *
   *  SqlQuery finder(*db);
   *  finder.setThrowOnFailure();
   *  string pntDist  = "distance(giscpt,UPCPoint(%longitude,%latitude))";
   *  string pntQuery = "SELECT pointid, latitude, longitude, radius FROM "
   *                    + pntTable + " WHERE (%distance <= " + QString(maxDist) +
   *                    ")";
   *
   *  Progress progress;
   *  progress.SetText("lodbnet");
   *  progress.SetMaximumSteps(pnts.rows());
   *  for (int row = 0 ; row < pnts.rows() ; row++) {
   *    CSVReader::CSVAxis pntR = pnts.getRow(row);
   *
   *    // Convert longitude to proper system if requested
   *    double longitude = QString(pntR[1]).ToDouble();
   *    if ((make360) && (longitude < 0.0)) { longitude += 360.0; }
   *    double latitude = QString(pntR[0]).ToDouble();
   *    double radius = QString(pntR[2]).ToDouble();
   *
   *    // Prepare the query, converting the longitude
   *    string dcheck(QString::Replace(pntDist, "%longitude",
   *    QString(longitude))); dcheck = QString::Replace(dcheck, "%latitude",
   *    QString(latitude)); string query = QString::Replace(pntQuery, "%distance",
   *    dcheck);
   *
   *    finder.exec(query);
   *    if (finder.size() > 0) {
   *      Statistics stats;
   *      vector<OutPoint> pointList;
   *      while (finder.next()) {
   *        SqlRecord record = finder.getRecord();
   *        OutPoint point;
   *        point.latitude = QString(record.getValue("latitude")).ToDouble(),
   *        point.longitude = QString(record.getValue("longitude")).ToDouble(),
   *        point.radius = QString(record.getValue("radius")).ToDouble(),
   *        point.pointid = QString(record.getValue("pointid")),
   *        stats.AddData(&point.radius, 1);
   *        pointList.push_back(point);
   *      }
   *    }
   *    progress.CheckStatus();
   *  }
   *
   * @endcode
   *
   * @see QString
   *
   * @ingroup Database
   *
   * @author 2006-11-09 Kris Becker
   *
   * @internal
   *  @history 2007-04-19 Kris Becker - Reordered return of getQuery() result
   *                                    to first try lastQuery(), then
   *                                    executedQuery().
   *  @history 2007-06-05 Brendan George - Modified to work with
   *           QString/StringTools merge
   *   @history 2008-10-30 Steven Lambright - tossQueryError now accepts a const
   *            char* for a filename, issue pointed out by "novus0x2a" (Support
   *            Board Member)
   */
  class SqlQuery : public QSqlQuery {
    public:
      SqlQuery();
      SqlQuery(Database &db);
      SqlQuery(const std::string &query,
               Database db = Database(Database::Connect));
      SqlQuery(const SqlQuery &other);
      virtual ~SqlQuery()  { }

      /**
       * @brief Report error status when executing queries
       *
       * @return bool True if iExceptions are thrown upon errors, otherwise
       *         returns false.
       */
      bool isThrowing() const {
        return (_throwIfFailed);
      }

      /**
       * @brief Sets throwing of exceptions on errors to true
       */
      void setThrowOnFailure() {
        _throwIfFailed = true;
      }
      /**
       * @brief Turns throwing of iExceptions off on errors
       */
      void setNoThrowOnFailure() {
        _throwIfFailed = false;
      }

      bool exec(const std::string &query);
      bool exec() {
        return(QSqlQuery::exec());
      }
      std::string getQuery() const;

      int nFields() const;
      std::string fieldName(int index) const;
      int fieldIndex(const std::string &name) const;
      std::vector<std::string> fieldNameList() const;
      std::vector<std::string> fieldTypeList() const;

      int nRows() const;
      SqlRecord getRecord() const;

    private:
      bool _throwIfFailed;        //!<  User can select action on query results

      void tossQueryError(const std::string &message, const char *f, int l) const;
  };
}
#endif
