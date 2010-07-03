#if !defined(SqlQuery_h)
#define SqlQuery_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $
 * $Date: 2008/10/30 16:40:50 $
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
#include "iException.h"
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
 *                    + pntTable + " WHERE (%distance <= " + iString(maxDist) +
 *                    ")";
 *
 *  Progress progress;
 *  progress.SetText("lodbnet");
 *  progress.SetMaximumSteps(pnts.rows());
 *  for (int row = 0 ; row < pnts.rows() ; row++) {
 *    CSVReader::CSVAxis pntR = pnts.getRow(row);
 *
 *    // Convert longitude to proper system if requested
 *    double longitude = iString(pntR[1]).ToDouble();
 *    if ((make360) && (longitude < 0.0)) { longitude += 360.0; }
 *    double latitude = iString(pntR[0]).ToDouble();
 *    double radius = iString(pntR[2]).ToDouble();
 *
 *    // Prepare the query, converting the longitude
 *    string dcheck(iString::Replace(pntDist, "%longitude",
 *    iString(longitude))); dcheck = iString::Replace(dcheck, "%latitude",
 *    iString(latitude)); string query = iString::Replace(pntQuery, "%distance",
 *    dcheck);
 *
 *    finder.exec(query);
 *    if (finder.size() > 0) {
 *      Statistics stats;
 *      vector<OutPoint> pointList;
 *      while (finder.next()) {
 *        SqlRecord record = finder.getRecord();
 *        OutPoint point;
 *        point.latitude = iString(record.getValue("latitude")).ToDouble(),
 *        point.longitude = iString(record.getValue("longitude")).ToDouble(),
 *        point.radius = iString(record.getValue("radius")).ToDouble(),
 *        point.pointid = iString(record.getValue("pointid")),
 *        stats.AddData(&point.radius, 1);
 *        pointList.push_back(point);
 *      }
 *    }
 *    progress.CheckStatus();
 *  }
 *
 * @endcode
 * 
 * @see iString
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
 *           iString/StringTools merge
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
      bool isThrowing() const { return (_throwIfFailed); }

      /**
       * @brief Sets throwing of exceptions on errors to true
       */
      void setThrowOnFailure() { _throwIfFailed = true; }
      /**
       * @brief Turns throwing of iExceptions off on errors
       */
      void setNoThrowOnFailure() { _throwIfFailed = false; }

      bool exec(const std::string &query);
      bool exec() { return(QSqlQuery::exec()); }
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

      void tossQueryError(const std::string &message, const char *f, int l) const
                          throw (iException &);
  };
}
#endif
