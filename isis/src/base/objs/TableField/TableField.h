#if !defined(TableField_h)
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

#include <string>
#include "PvlGroup.h"

namespace Isis {
 /**
  * @brief 
  *  
  * 
  * 
  * @ingroup LowLevelCubeIO
  * 
  * @author 2004-09-01 Jeff Anderson
  * 
  * @internal
  *   @history 2005-03-18 Elizabeth Ribelin - Added documentation to the class
  *   @history 2007-05-28 Steven Lambright - Added 4 byte
  *            floating point capabilities
  *   @todo Finish class documentation
  */
  class TableField {
    public:
      enum Type { Integer, Double, Text, Real };

      //Constructors and Destructor
      TableField(const std::string &name, Isis::TableField::Type type, 
                 int size=1);
      TableField(Isis::PvlGroup &field);
      ~TableField();

     /**
      * Returns the name of the TableField
      * 
      * @return Name of TableField
      */
      std::string Name() const { return p_name; };

     /**
      * Checks to see if field type is Integer
      * 
      * @return Returns true if field type is Integer, and false if it is not
      */
      bool IsInteger () const { return (p_type == TableField::Integer); };

     /**
      * Checks to see if field type is Double
      * 
      * @return Returns true if field type is Double, and false if it is not
      */
      bool IsDouble () const { return (p_type == TableField::Double); };

     /**
      * Checks to see if field type is Text
      * 
      * @return Returns true if field type is Text, and false if it is not
      */
      bool IsText () const { return (p_type == TableField::Text); };

     /**
      * Checks to see if field type is Text
      * 
      * @return Returns true if field type is Text, and false if it is not
      */
      bool IsReal () const { return (p_type == TableField::Real); };

     /**
      * Returns the number of bytes in the field
      * 
      * @return The number of bytes in the TableField
      */
      int Bytes() const { return p_bytes; };

     /**
      * Returns the size of the field
      * 
      * @return The size of the TableField
      */
      int Size() const { return p_size; };
  
      operator double() const;
      operator std::vector<double>() const;
      operator int() const;
      operator std::vector<int>() const;
      operator std::string() const;
      operator float() const;
      operator std::vector<float>() const;
  
      void operator=(const int value);
      void operator=(const double value);
      void operator=(const float value);
      void operator=(const std::string &value);
      void operator=(const std::vector<int> &values);
      void operator=(const std::vector<double> &values);
      void operator=(const std::vector<float> &value);
      void operator=(const char *buf);
      void operator=(const void *buf);
  
      Isis::PvlGroup PvlGroup();
  
    private:
      std::string p_name;             //!<Name of field
      Type p_type;                    //!<Type of field
      int p_size;                     //!<Size of field
      int p_bytes;                    //!<Number of bytes in field
      std::vector<int> p_ivalues;     //!<Vector of Integer values
      std::vector<double> p_dvalues;  //!<Vector of Double values
      std::vector<float> p_rvalues;  //!<Vector of Real values
      std::string p_svalue;           //!<string value of field
  };
};

#endif
