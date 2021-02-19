#ifndef SqlRecord_h
#define SqlRecord_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
