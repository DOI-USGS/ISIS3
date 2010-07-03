/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $
 * $Date: 2007/06/06 00:46:52 $
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
#include <cctype>
#include <string>
#include <vector>
#include <iostream>

#include "SqlRecord.h"
#include "SqlQuery.h"
#include "iString.h"

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
bool SqlRecord::hasField(const std::string &name) const {
  return (contains(iString::ToQt(name)));
}

/**
 * @brief Returns the name of a field/column at a particular index
 * 
 * For valid indexes, the name of the field/colunm is returned.  See the Qt
 * documentation on precise behavior.
 * 
 * @param index  Index of the desired column name to return.
 * 
 * @return std::string  Name of the column at the requested index
 */
std::string SqlRecord::getFieldName(int index) const {
  return (iString::ToStd(fieldName(index)));
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
int SqlRecord::getFieldIndex(const std::string &name) const {
  return(indexOf(iString::ToQt(name)));
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
 * @return std::string  Type of the field/column
 */
std::string SqlRecord::getType(const std::string &name) const {
  QVariant ftype(field(iString::ToQt(name)).type());
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
 * @see getType(const std::string &name).
 * 
 * @param index Index of the desired field/column to return type for.
 * 
 * @return std::string Type of the field/column at given index.
 */
std::string SqlRecord::getType(int index) const {
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
bool SqlRecord::isNull(const std::string &name) const {
  return (field(iString::ToQt(name)).isNull());
}

/**
 * @brief Returns the value of the field/column at specified index
 * 
 * This method will return the value of the field/column at the index.  It is
 * returned as a string and conversion handling is left to the caller.
 * 
 * @param index Index of field/column get value from.
 * 
 * @return iString Value of the field/column index.
 */
iString SqlRecord::getValue(int index) const {
  return (iString(iString::ToStd(field(index).value().toString())));
}

/**
 * @brief Returns the value of the named field/column
 * 
 * This method will return the value of the named field/column as a string.  It
 * is left to the caller to handle ant conversion of the returned type.
 * 
 * @param name  Name of the field/column to get value for.
 * 
 * @return iString Value of the named field/column.
 */
iString SqlRecord::getValue(const std::string &name) const {
  return (iString(iString::ToStd(field(iString::ToQt(name)).value().toString())));
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
 * @return std::string  A generic type for the Qt type.
 */
std::string SqlRecord::QtTypeField(const char *ctype) const {
  string retType("");
  if (ctype == 0) {
     return (retType);
  }
  else {
     retType = iString::DownCase((tolower(ctype[0]) == 'q') ? &ctype[1] : ctype);
  }
  return(retType);
}

}
