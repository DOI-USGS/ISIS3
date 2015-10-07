#ifndef Table_h
#define Table_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/14 19:17:09 $
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

#include "Blob.h"
#include <vector>
#include "TableRecord.h"

namespace Isis {
  class Pvl;
  /**
   * @brief Class for storing Table blobs information.
   *
   * This class can create new Tables or read table blobs from files. In 
   * general, records correspond to rows and fields correspond to columns. Thus 
   * the TableRecord class corresponds to a vector of row entries and 
   * TableField class corresponds to a specific entry of the table for a given 
   * record. Isis3 Table objects are record based, N records in a table. Each 
   * record will have the same number of fields, F. The fields can be of 
   * different types including Integer, Double, Text, and Real. The class 
   * uses PVL to store the structure of the table N, F, and Field types and 
   * binary to store the table data.
   *  
   * See the classes TableRecord and TableField for more information.
   *
   * If you would like to see Table being used in implementation, see histats.cpp
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2004-09-01 Jeff Anderson
   *
   * @internal
   *   @history 2005-03-18 Elizabeth Ribelin - Added documentation to class
   *   @history 2006-09-19 Jeff Anderson - Added clear method
   *   @history 2006-09-19 Jeff Anderson - Fixed bug in ReadInit method which needed to cleanup
   *                           pointers to records if a re-read occurred.
   *   @history 2009-02-18 Steven Lambright - Added copy constructor and assignment operator.
   *   @history 2011-05-25 Janet Barrett and Steven Lambright Added a Constructor that takes the pvl
   *                           labels so they do not have to be re-read, which is a very expensive
   *                           operation.
   *   @history 2012-10-04 Jeannie Backer Changed references to TableField methods to lower camel
   *                           case. Ordered includes. Added documentation. Added error check to
   *                           operator+=(TableRecord) if the Table's record size does not match the
   *                           added TableRecord's size. Fixed header definition statement. Added
   *                           forward declaration and includes. Fixed indentation of history
   *                           entries. Moved method implementation to cpp file. Ordered includes in
   *                           unitTest. Added RecordFields() accessor method. Improved test
   *                           coverage in all categories. Added padding to control statements.
   *                           References #1169.
   *   @history 2015-10-04 Jeannie Backer Improved coding standards. Uncommented error throw for
   *                           operator+=(record) that verifies that the record sizes match.
   *                           References #1178
   */
  class Table : public Isis::Blob {
    public:
      /** 
       *  
       */ 
      enum Association { 
             None, 
             Samples, 
             Lines, 
             Bands 
      };

      // Constructors and Destructors
      Table(const QString &tableName, TableRecord &rec);
      Table(const QString &tableName);// Only use this constructor for reading in an existing table
      Table(const QString &tableName, const QString &file);
      Table(const QString &tableName, const QString &file,
            const Pvl &fileHeader);
      Table(const Table &other);
      Table &operator=(const Isis::Table &other);

      ~Table();

      
      friend std::istream&operator>>(std::istream &is, Table &table);
      friend std::ostream&operator<<(std::ostream &os, Table &table);

      void SetAssociation(const Table::Association assoc);
      bool IsSampleAssociated();
      bool IsLineAssociated();
      bool IsBandAssociated();

      int Records() const;
      int RecordFields() const;
      int RecordSize() const;

      // Read a record
      TableRecord &operator[](const int index);

      // Add a record
      void operator+=(TableRecord &rec);

      // Update a record
      void Update(const TableRecord &rec, const int index);

      // Delete a record
      void Delete(const int index);

      void Clear();


      static QString toString(Table table, QString fieldDelimiter=",");

    protected:
      void ReadInit();
      void ReadData(std::istream &stream);
      void WriteInit();
      void WriteData(std::fstream &os);

      TableRecord p_record;          //!< The current table record
      std::vector<char *> p_recbufs; //!< Buffers containing record values

      int p_records; /**< Holds record count read from labels, may differ from
                         the size of p_recbufs.*/

      Association p_assoc; //!< Association Type of the table
      bool p_swap;         //!< Only used for reading
  };
};

#endif

