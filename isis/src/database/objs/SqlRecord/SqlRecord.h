#ifndef SqlRecord_h
#define SqlRecord_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2007/06/06 00:46:52 $
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
#include "IException.h"
#include <QSqlRecord>

class QString;

namespace Isis {

  class SqlQuery;

  /**
   * @brief Provide simplified access to resulting SQL query row
   *
   * This class is derived from
   * <a href="http://doc.trolltech.com/4.1/qsqlrecord.html">Qt's QSqlRecord</a>
   * class and is provided for convenience and simplifed use in a standard C++
   * environment.  Mainly, it provides strings and values as Standard QStrings
   * and other more common C++ constructs as well as taking advantage of some
   * unique Isis provisions (e.g., QString). One can still use Qt's rich features
   * interchangeably with this class.
   *
   * SqlRecord is intended to be used by the SqlQuery class provided in this
   * interface.
   *
   * @see SqlQuery.
   *
   * @ingroup Database
   *
   * @author 2006-08-18 Kris Becker
   *
   * @internal
   *   @history 2007-06-05 Brendan George - Modified to work with
   *                           QString/StringTools merge
   */
  class SqlRecord : public QSqlRecord {
    public:
      SqlRecord();
      SqlRecord(const SqlQuery &query);
      virtual ~SqlRecord()  { }

      /**
       * @brief Returns the number of fields/columns in query
       *
       * This result is the number of fields/columns returned in the query as a
       * result of the SQL statement issued to generate the resultant row set.
       *
       * @return int Number of fields/columns in row
       */
      int size() const {
        return (count());
      }

      bool hasField(const QString &name) const;
      int getFieldIndex(const QString &name) const;
      QString getFieldName(int index) const;

      QString getType(int index) const;
      QString getType(const QString &name) const;

      bool isNull(const QString &name) const;
      QString getValue(int index) const;
      QString getValue(const QString &name) const;

    private:
      QString QtTypeField(const char *ctype) const;

  };
}
#endif
