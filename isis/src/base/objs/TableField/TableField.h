#ifndef TableField_h
#define TableField_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2007/05/31 22:20:12 $
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

#include <vector>

#include <QString>

namespace Isis {
  class PvlGroup;
  /**
   * @brief Class for storing an Isis::Table's field information. 
   *  
   * This class represents the field values of an Isis3 table. In Isis3, fields 
   * correspond to column values.  Each TableField object is a single table 
   * entry for a column value at a specific row (or record) of the table. 
   *  
   * Be careful to note that the size of a field is the number of array values 
   * for a single column entry.  It is not the number of rows or records of the 
   * table. 
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2004-09-01 Jeff Anderson
   *
   * @internal
   *   @history 2005-03-18 Elizabeth Ribelin - Added documentation to the class
   *   @history 2007-05-28 Steven Lambright - Added 4 byte floating point
   *                           capabilities
   *   @history 2012-10-04 Jeannie Backer - Added accessor method for FieldType.
   *                           Moved method implementations to cpp file. Changed
   *                           member variable prefix to m_. Changed methods to
   *                           lower camel case. Added documentation. Moved
   *                           PvlGroup from header include to forward
   *                           declaration. Added includes to cpp and header.
   *                           Ordered includes and changed methods to lower
   *                           camel case in the unitTest. Improved test
   *                           coverage in all categories. Added padding to
   *                           control statements. Fixes #1169.
   *   @history 2012-11-21 Jeannie Backer - Added documentation and error
   *                           message if a Text type field is set to a string
   *                           value that is longer than the number of allowed
   *                           bytes for the field. References #700.
   *  
   *   @todo Finish class documentation
   */
  class TableField {
    public:
      /**
       * This enum describes the value type for the TableField.   
       */
      enum Type { 
             Integer, //!< The values in the field are 4 byte integers.
             Double,  //!< The values in the field are 8 byte doubles. 
             Text,    /**< The values in the field are text strings with 1 byte 
                           per character.*/
             Real     //!< The values in the field are 4 byte reals or floats.
      };

      //Constructors and Destructor
      TableField(const QString &name, Type type, int size = 1);
      TableField(PvlGroup &field);
      ~TableField();

      QString name() const;
      Type type() const;
      bool isInteger() const;
      bool isDouble() const;
      bool isText() const;
      bool isReal() const;
      int bytes() const;
      int size() const;

      operator int() const;
      operator double() const;
      operator float() const;
      operator QString() const;
      operator std::vector<int>() const;
      operator std::vector<double>() const;
      operator std::vector<float>() const;

      void operator=(const int value);
      void operator=(const double value);
      void operator=(const float value);
      void operator=(const QString &value);
      void operator=(const std::vector<int> &values);
      void operator=(const std::vector<double> &values);
      void operator=(const std::vector<float> &value);
      void operator=(const char *buf);
      void operator=(const void *buf);

      PvlGroup pvlGroup();

    private:
      QString m_name;            //!< Field name
      Type m_type;                   //!< Field value type
      int m_size;                    /**< Field size. This is the number of 
                                          values per field entry of the table.*/
      int m_bytes;                   //!< Number of bytes in field
      std::vector<int> m_ivalues;    /**< Vector containing integer field values.
                                          If the field Type is not Integer, this
                                          vector will be empty.*/
      std::vector<double> m_dvalues; /**< Vector containing double field values.
                                          If the field Type is not Double, this
                                          vector will be empty.*/
      std::vector<float> m_rvalues;  /**< Vector containing Real field values.
                                          If the field Type is not Real, this
                                          vector will be empty.*/
      QString m_svalue;          /**< String containing text value of field.
                                          If the field Type is not Text, this
                                          string will be empty.*/
  };
};

#endif
