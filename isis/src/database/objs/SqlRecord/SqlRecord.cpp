/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cctype>
#include <string>
#include <vector>
#include <iostream>

#include "IString.h"
#include "SqlRecord.h"
#include "SqlQuery.h"

#include <QSqlField>
#include <QVariant>

using namespace std;

namespace Isis {

  /**
   * @brief Default Constructor
   *
   * Construct a SqlRecord object with no content.  Not very useful, really.
   */
  SqlRecord::SqlRecord() : QSqlRecord() { }

  /**
   * @brief Construct a SqlRecord from a SqlQuery
   *
   * This constructor takes a SqlQuery object and constructs an object from the
   * current active row.  This is only valid after an initial call to the next()
   * method in the (derived) SqlQuery class.
   *
   * @param query A valid SqlQuery with an active row that will be accessable upon
   *              successful completion of this constructor.
   */
  SqlRecord::SqlRecord(const SqlQuery &query) : QSqlRecord(query.record()) { }

  /**
   * @brief Indicates the existance/non-existance of a field in the row
   *
   * This method can be used to determine if a field/column name exists within the
   * row.
   *
   * @param name Name of field/column to test for.  This is case insensitive.
   *
   * @return bool  True if the field/column exists, false otherwise.
   */
  bool SqlRecord::hasField(const QString &name) const {
    return (contains(name));
  }

  /**
   * @brief Returns the name of a field/column at a particular index
   *
   * For valid indexes, the name of the field/colunm is returned.  See the Qt
   * documentation on precise behavior.
   *
   * @param index  Index of the desired column name to return.
   *
   * @return QString  Name of the column at the requested index
   */
  QString SqlRecord::getFieldName(int index) const {
    return (fieldName(index));
  }

  /**
   * @brief Return the index of a named field/column
   * This method will determine the index of the named field after a query has
   * been successfully issued and results have been returned.
   *
   * @param name Name of field/column to return an index for
   *
   * @return int Index of the named column
   */
  int SqlRecord::getFieldIndex(const QString &name) const {
    return(indexOf(name));
  }

  /**
   * @brief Returns the type of a named field/column
   *
   * This method returns the type of a named column.  Types are derived from the
   * Qt type as defined
   * <a href="http://doc.trolltech.com/4.1/qvariant.html#Type-enum">here</a> in
   * the QVariant type description.
   *
   * What is returned is a type as named in the \a Description column of that
   * table without the \a Q and the resulting characters returned as a lower case
   * string.  For example the QVariant::Type of \b QChar is return as \b char.
   * The \b double type is returned as is, \b double.
   *
   * @see QtTypeField().
   * @see getType(int index).
   *
   * @param name Field/column name to determine type for
   *
   * @return QString  Type of the field/column
   */
  QString SqlRecord::getType(const QString &name) const {
    QVariant ftype(field(name).type());
    return (QtTypeField(ftype.typeName()));
  }

  /**
   * @brief Returns the type of a field/column at the specified index
   *
   * This method returns the type of a columnat the specfied index.  Types are
   * derived from the Qt type as defined
   * <a href="http://doc.trolltech.com/4.1/qvariant.html#Type-enum">here</a> in
   * the QVariant type description.
   *
   * What is returned is a type as named in the \a Description column of that
   * table without the \a Q and the resulting characters returned as a lower case
   * string.  For example the QVariant::Type of \b QChar is return as \b char.
   * The \b double type is returned as is, \b double.
   *
   * @see QtTypeField().
   * @see getType(const QString &name).
   *
   * @param index Index of the desired field/column to return type for.
   *
   * @return QString Type of the field/column at given index.
   */
  QString SqlRecord::getType(int index) const {
    QVariant ftype(field(index).type());
    return (QtTypeField(ftype.typeName()));
  }

  /**
   * @brief Determines if the value of the field/column is NULL
   *
   * @param name Name of field/column to check for NULL
   *
   * @return bool True if NULL, otherwise false.
   */
  bool SqlRecord::isNull(const QString &name) const {
    return (field(name).isNull());
  }

  /**
   * @brief Returns the value of the field/column at specified index
   *
   * This method will return the value of the field/column at the index.  It is
   * returned as a string and conversion handling is left to the caller.
   *
   * @param index Index of field/column get value from.
   *
   * @return QString Value of the field/column index.
   */
  QString SqlRecord::getValue(int index) const {
    return (QString(field(index).value().toString()));
  }

  /**
   * @brief Returns the value of the named field/column
   *
   * This method will return the value of the named field/column as a string.  It
   * is left to the caller to handle ant conversion of the returned type.
   *
   * @param name  Name of the field/column to get value for.
   *
   * @return QString Value of the named field/column.
   */
  QString SqlRecord::getValue(const QString &name) const {
    return (QString(field(name).value().toString()));
  }

  /**
   * @brief Returns a generic field type given a Qt QVariant type
   *
   * This routine converts the Qt QVariant
   * <a href="http://doc.trolltech.com/4.1/qvariant.html#Type-enum">type</a> to
   * a more generic type.  It is pretty simplistic in nature at this point.  It
   * will strip \a Q if it is the first character in the \a Description of the
   * type and convert the result to lower case.
   *
   * @param ctype A Qt QVariant::Type description
   *
   * @return QString  A generic type for the Qt type.
   */
  QString SqlRecord::QtTypeField(const char *ctype) const {
    QString retType("");
    if(ctype == 0) {
      return (retType);
    }
    else {
      retType = QString((tolower(ctype[0]) == 'q') ? &ctype[1] : ctype).toLower();
    }
    return(retType);
  }

}
