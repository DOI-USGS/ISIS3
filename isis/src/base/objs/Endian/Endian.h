/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/01/30 22:12:22 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#if !defined(Endian_h)
#define Endian_h

#include "iString.h"

namespace Isis {
  /**
   * @brief Tests the current architecture for byte order
   * 
   * Allows ISIS applications and objects to test the architecture's byte order.
   * Little Endian or Big Endian. Note: Middle Endian is not supported at this 
   * time.
   * 
   * @ingroup Utility
   * 
   * @author 2002-12-01 Tracie Sucharski
   * 
   * @internal
   *   @history 2003-02-11 Stuart Sides - Documented and created an object
   *                                      unittest
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
   *                                      isis.astrogeology...
   *   @history 2003-10-03 Stuart Sides - Added the byte order enumeration and
   *                                      functions to convert the enumeration 
   *                                      to a string and from a string to an
   *                                      enumeration. Also deprecated the
   *                                      LittleEndian and BigEndian functions.
   *                                      They were replaced by Lsb and Msb
   *                                      functions. 
   *   @history 2004-03-18 Stuart Sides - Added to macros for case where we need
   *                                      to know at compile time if a system is
   *                                      LSB or MSB. The macros are
   *                                      ISIS_LITTLE_ENDIAN and ISIS_BIG_ENDIAN
   */

  enum ByteOrder {
    NoByteOrder = 0,
    Lsb,
    Msb
  };

  inline std::string ByteOrderName (Isis::ByteOrder byteOrder) {
    if (byteOrder == Isis::NoByteOrder) return "None";
    if (byteOrder == Isis::Lsb) return "Lsb";
    if (byteOrder == Isis::Msb) return "Msb";
    return "Error";
  }

  inline Isis::ByteOrder ByteOrderEnumeration(const std::string &order) {
    Isis::iString temp(order);
    temp = temp.UpCase();
    if (temp == "LSB") return Isis::Lsb;
    if (temp == "MSB") return Isis::Msb;
    return Isis::NoByteOrder;
  }

  /**
   * Return true if this host is an LSB first machine and false if it is not
   * 
   * @return bool - Returns true if host is LSB and false if it is MSB
   */
  inline bool IsLsb(){
    union {
      short a;
      char b[2];
    } test;

    test.a = 1;
    if (test.b[0] == 0) return false;
    return true;
  }

  /**
   * Return true if this host is an MSB first machine and false if it is not
   * 
   * @return bool - Returns true if host is MSB and false if it is LSB
   */
  inline bool IsMsb() {
    return !Isis::IsLsb();
  }

  /**
   * @deprecated
   * 
   * Test the architecture the application is running on.
   * 
   * @return True if it's LittleEndian, False if it's not.
   */
  inline bool IsLittleEndian() {
    return IsLsb();
  }

  /**
   * @deprecated 
   * 
   * Test the architecture the application is running on.
   * 
   * @return True if it's BigEndian, False if it's not.
   */
  inline bool IsBigEndian() {
    return !Isis::IsLsb();
  }
}

#endif

