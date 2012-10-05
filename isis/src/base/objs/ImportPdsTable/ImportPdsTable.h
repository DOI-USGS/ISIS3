#ifndef ImportPdsTable_h
#define ImportPdsTable_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
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

#include <fstream>
#include <vector>
#include <string>

namespace Isis {
  class iString;
  class Table;
  class TableField;
  class TableRecord;
  class PvlObject;
  /**
   * @brief Import a PDS table file with a label description
   *
   * This class ingests a PDS table and converts it to an ISIS Table
   * object. This class can handle importing PDS tables whose data is BINARY 
   * or ASCII format.
   * The PDS label file (*.LBL) should completely define the contents of a
   * text file (*.TAB).  The name of the text file is determined from a label
   * keyword ^TABLE keyword.  Programmers can provide a different name for the
   * text table file.
   *
   * @ingroup Utility
   *
   * @author 2011-07-20 Kris Becker
   *
   * @internal 
   *   @history 2011-08-02 Kris Becker Modified to ensure the proper size of strings are
   *                         exported to the TableRecord
   *   @history 2012-10-04 Jeannie Backer Changed references to TableField
   *                           methods to lower camel case. Added and ordered
   *                           includes. Moved method implementation to cpp.
   *                           References #1169.
   */
  class ImportPdsTable {
    public:
      ImportPdsTable();
      ImportPdsTable(const std::string &labfile);
      ImportPdsTable(const std::string &labfile, const std::string &tabfile);
      ~ImportPdsTable() {}


      /** Return the number of columns */
      int columns() const;

      /** Return numnber of rows */
      int rows() const;

      void load(const std::string &labfile, const std::string &tabfile = "");
  
      bool hasColumn(const std::string &colName) const;
      std::string getColumnName(const int &index = 0, const bool &formatted = true)
                               const;
      std::vector<std::string> getColumnNames(const bool &formatted = true) const;
  
      std::string getType(const std::string &colName) const;
      bool setType(const std::string &colName, const std::string &dtype);
  
      Table exportAsTable(const std::string &tname) const;
      Table exportAsTable(const std::string &colNames,
                          const std::string &tname) const;
      Table exportAsTable(const std::vector<std::string> &colNames,
                          const std::string &tname) const;

    private:
      struct ColumnDescr {
        std::string m_name;       //!< Name of column
        int         m_colnum;     //!< Column number
        std::string m_type;       //!< Type of column
        int         m_sbyte;      //!< Starting byte of data
        int         m_nbytes;     //!< Number bytes in column
      };

      typedef std::vector<ColumnDescr> ColumnTypes;
      typedef std::vector<iString>     Columns;
      typedef std::vector<Columns>     Rows;

      //private instance variables
      int         m_trows;      //!< Number rows in table according to label
      ColumnTypes m_coldesc;    //!< Column descriptions
      Rows        m_rows;       //!<  Table data

      void init();

      void loadLabel(const std::string &labfile, std::string &tblfile);
      void loadTable(const std::string &tabfile);

      ColumnDescr getColumnDescription(PvlObject &colobj, int nth) const;
      ColumnDescr *findColumn(const std::string &colName);
      const ColumnDescr *findColumn(const std::string &colName) const;

      std::string getColumnValue(const std::string &tline,
                                 const ColumnDescr &cdesc) const;
      std::string getFormattedName(const std::string &colname) const;
      std::string getGenericType(const std::string &ttype) const;

      TableField makeField(const ColumnDescr &cdesc) const;
      TableRecord makeRecord(const ColumnTypes &ctypes) const;

      TableField &extract(const Columns &columns, const ColumnDescr &cdesc,
                          TableField &field) const;
      TableRecord &extract(const Columns &columns, const ColumnTypes &ctypes,
                           TableRecord &record) const;

      void fillTable(Table &table, const ColumnTypes &columns,
                     TableRecord &record) const;
  };

}
#endif

