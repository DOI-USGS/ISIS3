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

#include "TableField.h"
#include "iException.h"

using namespace std;
namespace Isis {

 /**
  * Constructs a TableField object
  * 
  * @param name The name of the field
  * 
  * @param type The type of the field
  * 
  * @param size The size of the field. Defaults to 0.
  */
  TableField::TableField(const std::string &name, Isis::TableField::Type type, 
                         int size) {
    p_name = name;
    p_type = type;
    p_size = size;
    if (p_type == TableField::Integer) {
      p_bytes = 4 * p_size;
      p_ivalues.resize(p_size);
    }
    else if (p_type == TableField::Double) {
      p_bytes = 8 * p_size;
      p_dvalues.resize(p_size);
    }
    else if (p_type == TableField::Text) {
      p_bytes = 1 * p_size; 
      p_svalue.resize(p_size);
    }
    else if (p_type == TableField::Real) {
      p_bytes = 4 * p_size;
      p_rvalues.resize(p_size);
    }
  }

 /**
  * Constructs a TableField object from a PvlGroup
  * 
  * @param field PvlGroup containing Name, Size, and Type for new TableField
  *              object
  * 
  * @throws Isis::iException::Programmer - Invalid field type
  */
  TableField::TableField(Isis::PvlGroup &field) {
    p_name = (string) field["Name"];
    p_size = (int) field["Size"];
    if ((string) field["Type"] == "Integer") {
      p_type = TableField::Integer;
      p_bytes = 4 * p_size;
      p_ivalues.resize(p_size);
    }
    else if ((string) field["Type"] == "Double") {
      p_type = TableField::Double;
      p_bytes = 8 * p_size;
      p_dvalues.resize(p_size);
    }
    else if ((string) field["Type"] == "Text"){
      p_type = TableField::Text;
      p_bytes = 1 * p_size; 
      p_svalue.resize(p_size);
    }
    else if ((string) field["Type"] == "Real") {
      p_type = TableField::Real;
      p_bytes = 4 * p_size;
      p_rvalues.resize(p_size);
    }
    else {
      string msg = "Field [" + p_name + "] has invalid type";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  }

  //! Destroys the TableField object
  TableField::~TableField() {
  }

 /**
  * 
  * 
  * @return 
  * 
  * @throws Isis::iException::Programmer - Field is not a Double
  */
  TableField::operator double() const {
    if (p_type != TableField::Double) {
      string msg = "Field [" + p_name + "] is not a Double";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_dvalues[0];
  }

 /**
  * 
  * 
  * @return
  * 
  * @throws Isis::iException::Programmer - Field is not an Integer
  */
  TableField::operator int() const {
    if (p_type != TableField::Integer) {
      string msg = "Field [" + p_name + "] is not Integer";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_ivalues[0];
  }

 /**
  * 
  * 
  * @return
  * 
  * @throws Isis::iException::Programmer - Field is not a Real
  */
  TableField::operator float() const {
    if (p_type != TableField::Real) {
      string msg = "Field [" + p_name + "] is not Real";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_rvalues[0];
  }

 /**
  * 
  * 
  * @return
  * 
  * @throws Isis::iException::Programmer - Field is not a string
  */
  TableField::operator std::string() const {
    if (p_type != TableField::Text) {
      string msg = "Field [" + p_name + "] is not Text";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_svalue;
  }

 /**
  * 
  * 
  * @return
  * 
  * @throws Isis::iException::Programmer - Field is not a Double array
  */
  TableField::operator std::vector<double>() const {
    if (p_type != TableField::Double) {
      string msg = "Field [" + p_name + "] is not a Double array";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_dvalues;
  }

 /**
  * 
  * 
  * @return 
  * 
  * @throws Isis::iException::Programmer - Field is not an Integer array
  */
  TableField::operator std::vector<int>() const {
    if (p_type != TableField::Integer) {
      string msg = "Field [" + p_name + "] is not an Integer array";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_ivalues;
  }

 /**
  * 
  * 
  * @return 
  * 
  * @throws Isis::iException::Programmer - Field is not an Integer array
  */
  TableField::operator std::vector<float>() const {
    if (p_type != TableField::Real) {
      string msg = "Field [" + p_name + "] is not a Real array";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_rvalues;
  }

 /**
  * Sets field value equal to input 
  * 
  * @param value Integer to be assigned to field value
  * 
  * @throws Isis::iException::Programmer - Field is not an Integer
  */
  void TableField::operator=(const int value) {
    if (p_type != TableField::Integer) {
      string msg = "Field [" + p_name + "] is not an Integer";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    p_ivalues[0] = value; 
  }

 /**
  * Sets field value equal to input
  * 
  * @param value Double to be assigned to field value
  * 
  * @throws Isis::iException::Programmer - Field is not a Double
  */
  void TableField::operator=(const double value) {
    if (p_type != TableField::Double) {
      string msg = "Field [" + p_name + "] is not a Double";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    p_dvalues[0] = value; 
  }

 /**
  * Sets field value equal to input
  * 
  * @param value string to be assigned to field value
  * 
  * @throws Isis::iException::Programmer - Field is not a string
  */
  void TableField::operator=(const std::string &value) {
    if (p_type != TableField::Text) {
      string msg = "Field [" + p_name + "] is not Text";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    p_svalue = value; 
  }

 /**
  * Sets field value equal to input
  * 
  * @param value float to be assigned to field value
  * 
  * @throws Isis::iException::Programmer - Field is not a string
  */
  void TableField::operator=(const float value) {
    if (p_type != TableField::Real) {
      string msg = "Field [" + p_name + "] is not Real";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    p_rvalues[0] = value;
  }

 /**
  * Sets field value equal to input
  * 
  * @param values Integer vector of values to be assigned to field value
  * 
  * @throws Isis::iException::Programmer - Field is not an Integer
  * @throws Isis::iException::Programmer - Vector is not the correct size
  */
  void TableField::operator=(const std::vector<int> &values) {
    if (p_type != TableField::Integer) {
      string msg = "Field [" + p_name + "] is not an Integer";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    else if ((int) values.size() != p_size) {
      string msg = "Field [" + p_name + "] values vector is not the correct size";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    p_ivalues = values; 
  }

 /**
  * Sets field value equal to the input
  * 
  * @param values Double vector of values to be assigned to field value
  * 
  * @throws Isis::iException::Programmer - Field is not a Double
  * @throws Isis::iException::Programmer - Vector is not the correct size
  */
  void TableField::operator=(const std::vector<double> &values) {
    if (p_type != TableField::Double) {
      string msg = "Field [" + p_name + "] is not a Double";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    else if ((int) values.size() != p_size) {
      string msg = "Field [" + p_name + "] values vector is not the correct size";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    p_dvalues = values; 
  }

 /**
  * Sets field value equal to the input
  * 
  * @param values Float vector of values to be assigned to field
  *               value
  * 
  * @throws Isis::iException::Programmer - Field is not a Real
  * @throws Isis::iException::Programmer - Vector is not the correct size
  */
  void TableField::operator=(const std::vector<float> &values) {
    if (p_type != TableField::Real) {
      string msg = "Field [" + p_name + "] is not a Real";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    else if ((int) values.size() != p_size) {
      string msg = "Field [" + p_name + "] values vector is not the correct size";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    p_rvalues = values; 
  }

 /**
  * 
  * 
  * @param ibuf
  * 
  * @throws Isis::iException::Programmer - Invalid field type
  */
  void TableField::operator=(const void *ibuf) {
    char *buf = (char *) ibuf;
    if (p_type == TableField::Double) {
      for (unsigned int i=0; i<p_dvalues.size(); i++) {
        // p_dvalues[i] = ((double *) buf)[i];
        double tmpd;
        memmove (&tmpd, buf+i*8, 8);
        p_dvalues[i] = tmpd;
      }
    }
    else if (p_type == TableField::Integer) {
      for (unsigned int i=0; i<p_ivalues.size(); i++) {
        // p_ivalues[i] = ((int *) buf)[i];
        int tmpi;
        memmove (&tmpi, buf+i*4, 4);
        p_ivalues[i] = tmpi;
      }
    }
    else if (p_type == TableField::Text) {
      p_svalue.resize(Bytes());
      for (int i=0; i<Bytes(); i++) {
        p_svalue[i] = buf[i];
      }
    }
    else if (p_type == TableField::Real) {
      for (unsigned int i=0; i<p_rvalues.size(); i++) {
        // p_ivalues[i] = ((int *) buf)[i];
        float tmpi;
        memmove (&tmpi, buf+i*4, 4);
        p_rvalues[i] = tmpi;
      }
    }
    else {
      string msg = "Invalid field type";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  }

 /**
  * 
  * 
  * @param buf
  * 
  * @throws Isis::iException::Programmer - Field is not Text
  */
  void TableField::operator=(const char *buf) {
    if (p_type != TableField::Text) {
      string msg = "Field [" + p_name + "] is not Text";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  
    p_svalue = buf;
  }

 /**
  * Creates and returns a PvlGroup named "Field" containing the following 
  * keywords and their respective values: "Name", "Type", and "Size"
  * 
  * @return PvlGroup containing field Name, Type, and Size
  */
  Isis::PvlGroup TableField::PvlGroup() {
    Isis::PvlGroup group("Field");
    group += Isis::PvlKeyword("Name",p_name);
    if (p_type == TableField::Double) {
      group += Isis::PvlKeyword("Type","Double");
    }
    else if (p_type == TableField::Integer) {
      group += Isis::PvlKeyword("Type","Integer");
    }
    else if (p_type == TableField::Text) {
      group += Isis::PvlKeyword("Type","Text");
    }
    else if (p_type == TableField::Real) {
      group += Isis::PvlKeyword("Type","Real");
    }
    group += Isis::PvlKeyword("Size",p_size);
  
    return group;
  }
} // end namespace isis

