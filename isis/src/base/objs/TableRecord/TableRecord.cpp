/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/06/25 18:13:35 $
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
                                                                     

#include "TableRecord.h"
#include "iString.h"
#include "iException.h"
    
using namespace std;
namespace Isis {

 /**
  * Returns the number of bytes per record
  * 
  * @return Number of bytes per record
  */
  int TableRecord::RecordSize() const {
    int bytes = 0;
    for (int i=0; i<(int)p_fields.size(); i++) bytes += p_fields[i].Bytes();
    return bytes;
  }

 /**
  * Returns the TableField in the record whose name corresponds to the 
  * input string 
  * 
  * @param field The name of desired TableField
  * 
  * @return The specified TableField
  * 
  * @throws Isis::iException::Programmer - The field does not exist in the 
  *                                        record
  */
  Isis::TableField &TableRecord::operator[](const std::string &field) {
    Isis::iString upTemp = field;
    upTemp.UpCase();
    for (int i=0; i<(int)p_fields.size(); i++) {
      Isis::iString upField = p_fields[i].Name();
      upField.UpCase();
      if (upTemp == upField) return p_fields[i];
    }
  
    string msg = "Field [" + field + "] does not exist in record";
    throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
  }
  
 /**
  * 
  * 
  * @param buf 
  * 
  * @throws Isis::iException::Programmer - Invalid field type
  */
  void TableRecord::Pack(char *buf) const {
    int sbyte = 0;
    for (int f=0; f<Fields(); f++) {
      const Isis::TableField &field = p_fields[f];
      if (field.IsDouble()) {
        vector<double> vals = field;
        for (unsigned int i=0; i<vals.size(); i++) {
          //*((double *)(buf+sbyte)) = vals[i];
          memmove (buf+sbyte, &vals[i], 8);
          sbyte += sizeof(double);
        }
      }
      else if (field.IsInteger()) {
        vector<int> vals = field;
        for (unsigned int i=0; i<vals.size(); i++) {
          //*((int *)(buf+sbyte)) = vals[i];
          memmove (buf+sbyte, &vals[i], 4);
          sbyte += sizeof(int);
        }
      }
      else if (field.IsText()) {
        string val = (string)field;
        for( int i=0; i<field.Size(); i++ ) {
          if( i < (int)val.length() ) {
            buf[sbyte] = val[i];
          }
          else {
            buf[sbyte] = 0;
          }
          sbyte++;
        }
      }
      else if (field.IsReal()) {
        vector<float> vals = field;
        for (unsigned int i=0; i<vals.size(); i++) {
          //*((int *)(buf+sbyte)) = vals[i];
          memmove (buf+sbyte, &vals[i], 4);
          sbyte += sizeof(float);
        }
      }
      else {
        string msg = "Invalid field type";
        throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
      }
    }
  }

 /**
  * 
  * 
  * @param buf
  */
  void TableRecord::Unpack(const char *buf) {
    int sbyte = 0;
    for (int f=0; f<Fields(); f++) {
      Isis::TableField &field = p_fields[f];
      field = (void *)&buf[sbyte];
      sbyte += field.Bytes();
    }
  }
  
 /**
  * 
  * 
  * @param buf
  * 
  * @throws Isis::iException::Programmer - Invalid field type
  */
  void TableRecord::Swap(char *buf) const {
    int sbyte = 0;
    for (int f=0; f<Fields(); f++) {
      const Isis::TableField &field = p_fields[f];
      if (field.IsDouble()) {
        for (int i=0; i<field.Size(); i++) {
          char *swap = &buf[sbyte];
          char temp;
          temp = swap[0]; swap[0] = swap[7]; swap[7] = temp;
          temp = swap[1]; swap[1] = swap[6]; swap[6] = temp;
          temp = swap[2]; swap[2] = swap[5]; swap[5] = temp;
          temp = swap[3]; swap[3] = swap[4]; swap[4] = temp;
  
          sbyte += sizeof(double);
        }
      }
      else if (field.IsInteger()) {
        for (int i=0; i<field.Size(); i++) {
          char *swap = &buf[sbyte];
          char temp;
          temp = swap[0]; swap[0] = swap[3]; swap[3] = temp;
          temp = swap[1]; swap[1] = swap[2]; swap[2] = temp;
          sbyte += sizeof(int);
        }
      }
      else if (field.IsText()) {
        sbyte += field.Bytes();
      }
      else if (field.IsReal()) {
        for (int i=0; i<field.Size(); i++) {
          char *swap = &buf[sbyte];
          char temp;
          temp = swap[0]; swap[0] = swap[3]; swap[3] = temp;
          temp = swap[1]; swap[1] = swap[2]; swap[2] = temp;
          sbyte += sizeof(float);
        }
      }
      else {
        string msg = "Invalid field type";
        throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
      }
    }
  }
} // end namespace isis
