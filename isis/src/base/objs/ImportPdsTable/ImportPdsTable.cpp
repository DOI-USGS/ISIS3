/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2005/08/18 23:00:25 $
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
#include "ImportPdsTable.h"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "EndianSwapper.h"
#include "FileName.h"
#include "IString.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "Table.h"
#include "TableField.h"
#include "TableRecord.h"
#include "TextFile.h"


using namespace std;

namespace Isis {


  /** Default constructor */
  ImportPdsTable::ImportPdsTable() {
    // just inititalize the member variables
    init();
  }

  /**
   * @brief Constructor accepts the name of the label file
   *
   * This constructor takes the name of the label file describing the PDS table.
   * It will extract the description of the columns and the name of the table 
   * data file.  The table data file is also read and internalized.
   *
   * @param labfile Name of table label file
   */
  ImportPdsTable::ImportPdsTable(const std::string &labfile) {
    string tblfile;
    load(labfile, tblfile);
  }

  /**
   * @brief Constructor automatically loads the label and table files 
   *
   * This constructor takes the name of the label file describing the PDS table
   * and the table data file name.  It will extract the description of the 
   * columns and read the contents of the table data file.
   *
   * @param labfile Name of table label file
   * @param tabfile Name of table data file
   */
  ImportPdsTable::ImportPdsTable(const std::string &labfile,
                                 const std::string &tabfile) {
    load(labfile, tabfile);
  }

  /**
   * @brief Loads a PDS table label and (optional) data file
   *
   * This method will load a PDS table dataset using a label file describing the
   * contents of the table data.  The caller can provide the table data file,
   * otherwise, the name of the table data file is extracted from label in the
   * ^TABLE keyword.  The table data is then loaded.
   *
   * when this method is invoked, the current contents of the object are
   * discarded.
   *
   *
   * @param labfile Name of table label file
   * @param tabfile Name of table data file (optional)
   */
  void ImportPdsTable::load(const std::string &labfile,
                            const std::string &tabfile) {

    init();
    string tblfile;
    loadLabel(labfile, tblfile);
    if (!tabfile.empty()) tblfile = tabfile;
    loadTable(tblfile);
    return;
  }

  /** Determine if a named column exists */
  bool ImportPdsTable::hasColumn(const std::string &colName) const {
     return (findColumn(colName) != 0);
  }


  /**
   * @brief Return the name of the specifed column
   *
   * This method will return the name of a specified column by index. It also 
   * has the option to format the column name to Camel-Case.  This will remove 
   * all left and right parens, convert white space to spaces, compress 
   * consecutive spaces to only one space.  It then removes the spaces 
   * converting the next character to uppercase.
   *
   * @param index      Index of colunm name to get.
   * @param formatted  Specifies to convert the name to Camel-Case if true,
   *                   otherwise leave as is in the PDS table.
   *
   * @return std::string Returns the column name as requested
   */
  std::string ImportPdsTable::getColumnName(const int &index,
                                            const bool &formatted) const {
    if ((index < 0) || (index >= columns()) ) {
      ostringstream mess;
      mess << "Requested column index (" << index
           << "exceeds numnber of columns (" << columns() << ")";
      throw IException(IException::Programmer, mess.str(), _FILEINFO_);
    }
    string name = m_coldesc[index].m_name;
    if (formatted) name = getFormattedName(name);
    return (name);
  }

  /**
   * @brief Return the names of all the columns
   *
   * This method will return the names of all columns. It also has the option to
   * format the column name to Camel-Case.  This will remove all left and right
   * parens, convert white space to spaces, compress consecutive spaces to only
   * one space. It then removes the spaces converting the next character to
   * uppercase.
   *
   * @param formatted  Specifies to convert the name to Camel-Case if true,
   *                   otherwise leave as is in the PDS table.
   *
   * @return std::vector<std::string> Returns vector of all column names
   */
  std::vector<std::string> ImportPdsTable::getColumnNames(const bool &formatted)
                                                          const {
    vector<string> colnames;
    for (unsigned int i = 0 ; i < m_coldesc.size() ; i++) {
      string name = m_coldesc[i].m_name;
      if (formatted) name = getFormattedName(name);
      colnames.push_back(name);
    }
    return (colnames);
  }

  /**
   * @brief Get the type associated with the specified column
   *
   * This method returns the datatype associated with the specfied column.  If 
   * the column does not exist, an empty string is returned.
   *
   * @author kbecker (6/27/2011)
   *
   * @param colName      Name of column to get type for
   *
   * @return std::string Returns the type of the column.  If the column does not
   *         exist, an empty string is returned.
   */
  std::string ImportPdsTable::getType(const std::string &colName) const {
    const ColumnDescr *column = findColumn(colName);
    string dtype("");
    if (column != 0) {
      dtype = column->m_type;
    }
    return (dtype);
  }

  /**
   * @brief Change the datatype for a column
   *
   * This method changes the data type the specified column.
   *
   * @author kbecker (6/27/2011)
   *
   * @param colName Name of column to change
   * @param dtype   New type of column.  Support types are DOUBLE, REAL, INTEGER
   *                and CHARACTER.  Unsupported/unknown types are treated as
   *                CHARACTER.
   *
   * @return bool
   */
  bool ImportPdsTable::setType(const std::string &colName,
                               const std::string &dtype) {
    ColumnDescr *column = findColumn(colName);
    if (column != 0) {
      column->m_type = IString(dtype).UpCase();
    }
    return (column != 0);
  }


  /**
   * @brief Populate a Table object with the PDS table and return it
   *
   * This method converts all the PDS table data to an ISIS table.
   *
   * @param tname Name of table
   *
   * @return Table Table containing PDS table data
   */
  Table ImportPdsTable::exportAsTable(const std::string &tname) const {
    TableRecord record = makeRecord(m_coldesc);
    Table table(tname, record);
    fillTable(table, m_coldesc, record);
    return (table);
  }


  /**
   * @brief Populate ISIS Table with specified column(s)
   *
   * This method extracts columns specified by the caller in a string.  It is
   * typically used for a single column, but any number of columns can be
   * provided.  colnames is a comma delimited string that contains the name of
   * the columns that will be exported in the table.
   *
   * @param colNames String containing comma delimited column names to export
   * @param tname    Name of table to create
   *
   * @return Table  Table containing the specified columns
   */
  Table ImportPdsTable::exportAsTable(const std::string &colnames,
                                      const std::string &tname) const {
    std::vector<std::string> cols;
    IString::Split(',', colnames, cols);
    return (exportAsTable(cols, tname));
  }

  /**
   * @brief Populate ISIS Table with specific columns
   *
   * This method extracts columns specified by the caller.  If the requested
   * column does not exist, an exception is thrown.
   *
   *
   * @param colNames Vector column names to convert to a table.
   * @param tname  Name of the table to create.
   *
   * @return Table
   */
  Table ImportPdsTable::exportAsTable(const std::vector<std::string> &colnames,
                                      const std::string &tname) const {
    ColumnTypes ctypes;
    for (unsigned int i = 0 ; i < colnames.size() ; i++) {
      const ColumnDescr *descr = findColumn(colnames[i]);
      if (!descr) {
        ostringstream mess;
        mess << "Requested column name (" << colnames[i] << ") does not exist in table";
        throw IException(IException::Programmer, mess.str(), _FILEINFO_);
      }
      ctypes.push_back(*descr);
    }

    // Create and populate the table
    TableRecord record = makeRecord(ctypes);
    Table table(tname, record);
    fillTable(table, ctypes, record);
    return (table);
  }


  /** Initialize object variables */
  void ImportPdsTable::init() {

    m_trows = 0;
    m_coldesc.clear();
    m_rows.clear();
    return;
  }

  /**
   * @brief Loads the contents of a PDS table label description
   *
   * The labfile parameter contains the name of a PDS label describing the
   * contents of a PDS table data file.  It will be loaded and parsed by this
   * method. The name of the table data file is returned in the tblfile parameter.
   * It is not loaded.
   *
   *
   * @param labfile Name of PDS table label description file
   * @param tblfile Returns the name of the PDS table data
   */
  void ImportPdsTable::loadLabel(const std::string &labfile, std::string &tblfile) {

    Pvl label(labfile);

    if (!label.HasObject("TABLE")) {
      std::string msg = "File " + labfile +
                        " does not have the required TABLE object, probably not"
                        " a valid PDS table label!";
      throw IException(IException::User, msg, _FILEINFO_);
    }

  //  Get some pertinent information from the label
    PvlObject &tabobj = label.FindObject("TABLE");
    tblfile = FileName(labfile).path() + "/" + label["^TABLE"][0];

    m_trows = (int) tabobj.FindKeyword("ROWS");
    int ncols =  (int) tabobj.FindKeyword("COLUMNS");

    m_coldesc.clear();
    PvlObject::PvlObjectIterator colobj = tabobj.BeginObject();
    int icol(0);
    while (colobj != tabobj.EndObject()) {
      if (colobj->IsNamed("COLUMN")) {
        m_coldesc.push_back(getColumnDescription(*colobj, icol));
        icol++;
      }
      ++colobj;
    }

    //  Test to ensure columns match the number listed in the label
    if (ncols != (int) m_coldesc.size()) {
      ostringstream msg;
      msg << "Number of columns in the COLUMNS label keyword (" << ncols
           << ") does not match number of COLUMN objects found ("
           << m_coldesc.size() << ")";
  #if 0
      throw iException::Message(iException::Programmer, msg.str(), _FILEINFO_);
  #else
       cout << msg.str() << "\n";
  #endif
    }
    return;
  }

  /**
   * @brief Loads the contents of a PDS table data file
   *
   * This method open and read the contents of a PDS table data file.  Values 
   * for each column are extracted from each row according to the label 
   * description. The entire table contents are stored internally. 
   *
   * Note that the table label description must already be loaded in this 
   * object. 
   * @see loadLabel().
   *
   * @param tabfile Name of PDS table data file
   */
  void ImportPdsTable::loadTable(const std::string &tabfile) {

    //  Vet the filename.  Many PDS files record the filename in uppercase and
    //  in practice, the filename is in lowercase.  Do this check here.
    string tblfile(tabfile);
    FileName tname(tblfile);
    if (!tname.fileExists()) {
      tname =  tname.path() + "/" + IString(tname.name()).DownCase();
      tblfile = tname.expanded();
    }

    TextFile tfile(tblfile);
    string tline;
    m_rows.clear();
    int irow(0);
    while (tfile.GetLine(tline, false)) {
      if (irow >= m_trows) break;

      Columns columns;
      for (unsigned int i = 0 ; i < m_coldesc.size() ; i++) {
        columns.push_back(getColumnValue(tline, m_coldesc[i]));
      }
      m_rows.push_back(columns);
      irow++;
    }
    return;
  }


  /**
   * @brief Extract a column description from a COLUMN object
   *
   * This method will extract a column description from a COLUMN object.  This
   * object will typically be contained in a Pvl PDS compatable file that
   * accompanies the table data file.
   *
   * The keywords NAME, DATA_TYPE, START_BYTE and BYTES must exist.
   *
   * @param colobj Pvl Object containing column description
   * @param nth    Current column counter
   *
   * @return ImportPdsTable::ColumnType Returns internal struct of column
   *         description
   */
  ImportPdsTable::ColumnDescr ImportPdsTable::getColumnDescription(PvlObject &colobj,
                                                                   int nth) const {
    ColumnDescr cd;
    cd.m_name = colobj["NAME"][0];
    cd.m_colnum = nth;
    cd.m_type = IString(getGenericType(colobj["DATA_TYPE"][0])).UpCase();
    cd.m_sbyte = ((int) colobj["START_BYTE"]) - 1;   // 0-based indexing
    cd.m_nbytes = (int) colobj["BYTES"];
    return (cd);
  }

  /**
   * @brief Searches internal column descriptors for a named column
   *
   * This method converts the column names to a formatted name for consistency 
   * and then checks for the given name - with case insensitivity.  If found, a
   * pointer to the column description is returned, otherwise NULL.
   *
   *
   * @param colName Name of column to find
   *
   * @return ImportPdsTable::ColumnDescr* Pointer to column description is
   *         returned if found otherwise, NULL is returned.
   */
  ImportPdsTable::ColumnDescr *ImportPdsTable::findColumn(
      const std::string &colName) {
    string cName = getFormattedName(colName);
    ColumnTypes::iterator col = m_coldesc.begin();
    while (col != m_coldesc.end()) {
      string c = getFormattedName(col->m_name);
      if (IString::Equal(c, cName)) { return (&(*col));  }
      col++;
    }
    return (0);
  }

  /**
   * @brief Searches internal column descriptors for a named column
   *
   * This method converts the column names to a formatted name for consistency 
   * and then checks for the given name - with case insensitivity.  If found, a
   * pointer to the column description is returned, otherwise NULL.
   *
   *
   * @param colName Name of column to find
   *
   * @return ImportPdsTable::ColumnDescr* Pointer to column description is
   *         returned if found otherwise, NULL is returned.
   */
  const ImportPdsTable::ColumnDescr *ImportPdsTable::findColumn(
      const std::string &colName) const {
    string cName = getFormattedName(colName);
    ColumnTypes::const_iterator col = m_coldesc.begin();
    while (col != m_coldesc.end()) {
      string c = getFormattedName(col->m_name);
      if (IString::Equal(c, cName)) { return (&(*col));  }
      col++;
    }
    return (0);
  }


  /**
   * @brief Extracts a column from a string based upon a description
   *
   *
   * @param tline  Row from table data
   * @param cdesc  Column description
   *
   * @return std::string Returns the extracted column as a string
   */
  std::string ImportPdsTable::getColumnValue(const std::string &tline,
                                             const ColumnDescr &cdesc) const {
    return (tline.substr(cdesc.m_sbyte, cdesc.m_nbytes));
  }

  /**
   * @brief Converts a column name to a camel-case after it has been cleansed
   *
   * This method will convert a column name to camel-case after some character
   * cleaning is performed.  All white space characters as defined by the 
   * IString class are converted to spaces.  Spaces are then compressed to one 
   * space. Any left/right parens are removed.  Then the conversion to 
   * camel-case is performed.
   *
   * Camel case always converts the first character in a string to uppercase. 
   * Any space or '_' character are removed and the following character is 
   * converted to uppercase.  All other characters are converted to lowercase. 
   *
   * @param colname Column name to converty
   *
   * @return std::string Returns the formatted keyword
   */
  std::string  ImportPdsTable::getFormattedName(
      const std::string &colname) const {

    IString cname = IString::ConvertWhiteSpace(colname);
    cname.Remove("(,)");
    cname.Compress();

    bool uppercase = true;
    string ostring;
    for (unsigned int i = 0 ; i < cname.size() ; i++) {
      if (uppercase) {
        ostring.push_back(toupper(cname[i]));
        uppercase = false;
      }
      else if ( (cname[i] == ' ') || (cname[i] == '_') ) {
        uppercase = true;
      }
      else {
        ostring.push_back(tolower(cname[i]));
      }
    }

    return (ostring);
  }


  /**
   * @brief Determine generic data type of a column
   *
   * This method will determine the generic type of a column.  In general, PDS
   * table descriptions contain types that have some prepended qualifiers to
   * column data types.  This routine removes those qualifiers and only returns
   * the generic type.
   *
   * For example, if the incoming type is MSB_INTEGER, only INTEGER will be
   * returned.
   *
   * @param ttype PDS data type to convert
   *
   * @return std::string Generic type found in the data type.
   */
  std::string ImportPdsTable::getGenericType(const std::string &ttype) const {
    vector<string> parts;
    int n = IString::Split('_', ttype, parts);
    return (parts[n-1]);
  }


  /**
   * @brief Creates a TableField for the column type
   *
   * This method generates a TableField with the appropriate type to contain the
   * PDS coulumn data.  It will only create three different types:  Integer,
   * Double or Text.
   *
   * All PDS data types that have INTEGER in their type are stored as an Integer
   * field type.  PDS columns with DOUBLE, REAL or FLOAT are stored as Doubles.
   * All other types are Text types.
   *
   * @param cdesc Column description to create the TableField from
   *
   * @return TableField Returns a TableField for the column type
   */
  TableField ImportPdsTable::makeField(const ColumnDescr &cdesc) const {
    string dtype = cdesc.m_type;
    string name = getFormattedName(cdesc.m_name);
    if ( dtype == "INTEGER" ) {
      return (TableField(name, TableField::Integer));
    }
    else if ( ((dtype == "DOUBLE" ) ||(dtype == "REAL") || (dtype == "FLOAT")) ) {
      return (TableField(name, TableField::Double));
    }

  // All other cases are just text
    return (TableField(name, TableField::Text, cdesc.m_nbytes));
  }

  /**
   * @brief Creates a TableRecord for columns
   *
   * This method creates a TableRecord for each column in the table.  This 
   * record should be provided when creating the ISIS Table object.  It can also 
   * be used to populate the table.  TableFields are added in the order provided 
   * in ctypes. 
   *
   * @param ctypes  Columns to create fields for and add to record
   *
   * @return TableRecord Returns TableRecord of columns/fields
   */
  TableRecord ImportPdsTable::makeRecord(const ColumnTypes &ctypes) const {
    TableRecord rec;
    for (unsigned int i = 0 ; i < ctypes.size() ; i++) {
      TableField field = makeField(ctypes[i]);
      rec += field;
    }
    return (rec);
  }


  /**
   * @brief Extract a TableField from a PDS column in the text row
   *
   * This routine will extract the column and convert to a TableField with the
   * appropriate type to contain the field.
   *
   *
   * @param columns Columns for a given row
   * @param cdesc   Column description used to create TableField
   *
   * @return TableField  Returns the TableField with the value from the column
   */
  TableField &ImportPdsTable::extract(const Columns &columns,
                                      const ColumnDescr &cdesc,
                                      TableField &tfield) const {
    int ith = cdesc.m_colnum;
    try {
      IString data(columns[ith]);
      if (tfield.isInteger()) {
        data.Trim(" \t\r\n");
        tfield = data.ToInteger();
      }
      else if (tfield.isDouble()) {
        data.Trim(" \t\r\n");
        tfield = data.ToDouble();
      }
      else {  // Its a text field
        string str(tfield.size(), ' ');
        str.insert(0, data.Trim(" \t\r\n"));
        tfield = str;
      }
    }
    catch (IException &e) {
      string mess = "Conversion failure of column " + cdesc.m_name;
      throw IException(e, IException::Programmer, mess, _FILEINFO_);
    }

    return (tfield);
  }

  /**
   * @brief Extract a table record of columns from a row of columns
   *
   * This method will create a TableRecord from a list of columns.  The columns
   * are selected from the list of column descriptions parameter.
   *
   *
   * @param columns Row of column data
   * @param ctypes  List of columns to extract
   * @param record  TableRecord with fields to populate
   *
   * @return TableRecord Record containing extract fields from columns
   */
  TableRecord &ImportPdsTable::extract(const Columns &columns,
                                       const ColumnTypes &ctypes,
                                       TableRecord &record) const {
    for (unsigned int i = 0 ; i < ctypes.size() ; i++) {
      extract(columns, ctypes[i], record[i]);
    }
    return (record);
  }


  /**
   * @brief Fill the ISIS Table object with PDS table data
   *
   * This method populates the ISIS Table object with selected PDS table data
   * fields.
   *
   * @param table   ISIS Table object to populate
   * @param columns PDS Column map to extract data
   * @param record  ISIS TableRecord with fields to contain PDS columns
   */
  void ImportPdsTable::fillTable(Table &table, const ColumnTypes &columns,
                                 TableRecord &record) const {
    for (unsigned int i = 0 ; i < m_rows.size() ; i++) {
      try {
        table += extract(m_rows[i], columns, record);
      }
      catch (IException &e) {
        string mess = "Failed to convert data in row " + IString((int) i);
        throw IException(e, IException::Programmer, mess, _FILEINFO_);
      }
    }
    return;
  }

  int ImportPdsTable::columns() const { 
    return (m_coldesc.size()); 
  }

  int ImportPdsTable::rows() const { 
    return (m_rows.size()); 
  }

}  //  namespace Isis

