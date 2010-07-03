/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2007/01/30 22:12:23 $
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
#include "PvlToken.h" 
#include "iException.h"
#include "Message.h"

using namespace std;
namespace Isis {
 /** 
  * Constructs a Token with k for keyword and NULL for the value list.
  * 
  * @param k Value of the keyword
  */
  PvlToken::PvlToken (const std::string &k) { 
    SetKey (k);
    ValueClear (); 
  } 
 
  //! Constructs a Token with NULL for both the keyword and value list.
  PvlToken::PvlToken () { 
    key.erase();
    ValueClear (); 
  }

  //! Destroys the Token object
  PvlToken::~PvlToken () { 
    key.erase();
    ValueClear (); 
  }

 /** 
  * Set the token keyword. 
  * 
  * @param k iString to load into the token keyword
  */
  void PvlToken::SetKey (const std::string &k) { 
    key = k;
  };

 /** 
  * Returns the token keyword
  * 
  * @return string
  */
  string PvlToken::GetKey () const { 
    return key; 
  };

 /** 
  * Returns the token keyword in all uppercase characters  
  * 
  * @return string
  */
  string PvlToken::GetKeyUpper () const { 
    string upperkey = key;
    for (int i=0; i<(int)upperkey.size(); i++) {
      upperkey[i] = toupper (upperkey[i]);
    }
    return upperkey; 
  }; 

  //! Removes all elements from the value-vector
  void PvlToken::ValueClear () { 
    value.clear (); 
  }

 /** 
  * Returns the number of elements in the value-vector
  * 
  * @return int
  */
  int PvlToken::ValueSize () const { 
    return value.size (); 
  }

 /** 
  * Adds a value to the value-vector. Successive calls add values to the end of 
  * the vector.
  * 
  * @param v iString add to the value-vector list 
  */
  void PvlToken::AddValue (const std::string &v) { 
    value.push_back (v); 
  }

 /** 
  * Returns one element of the value-vector.
  *
  * @param index Zero-based index of vector element to return. Defaults to 0
  * 
  * @return string   
  * 
  * @throws Isis::iException::Programmer
  */
  string PvlToken::GetValue (const int index) const { 
    if ((index < 0) || (index >= (int) value.size())) {
      string message = Isis::Message::ArraySubscriptNotInRange(index); 
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_);
    }
    return value[index];
  }

 /** 
  * Returns one element of the value-vector in uppercase.
  *
  * @param index Zero-based index of vector element to return. Defaults to 0
  * 
  * @return string
  *  
  * @throws Isis::iException::Programmer
  */
  string PvlToken::GetValueUpper (const int index) const { 
    if ((index < 0) || (index >= (int) value.size())) {
      string message = Isis::Message::ArraySubscriptNotInRange(index); 
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_);
    }
    string upper = value[index];
    for (int i=0; i<(int)upper.size(); i++) {
      upper[i] = toupper (upper[i]);
    }
    return upper; 
  }
} // end namespace isis
