#if !defined(PixelType_h)
#define PixelType_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2007/01/30 22:12:22 $
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
#include "iString.h"

namespace Isis {
 /**
  * @brief Enumerations for Isis Pixel Types
  * 
  * This enumeration is for defining pixel types of cubes. UnsignedByte, 
  * SignedWord, and Real are the only three pixel types currently supported.
  * The range for UnsignedBytes is 0 to 255, the range for SignedWord is -32768
  * to 32767, and the range for Real is -FLT_MAX to FLT_MAX as defined in 
  * the include file, float.h.
  * 
  * @author 2004-10-14 Stuart Sides
  * 
  * @internal
  *   @history 2005-03-01 Elizabeth Ribelin - Added documentation in Doxygen 
  *                                           format
  */
  enum PixelType {
    None=0,
    UnsignedByte,   
    SignedByte,
    UnsignedWord,
    SignedWord,
    UnsignedInteger,
    SignedInteger,
    Real,
    Double
  };

 /**
  * Returns the number of bytes of the specified PixelType
  * 
  * @param pixelType PixelType enumeration
  * 
  * @return int Size of pixel type byte
  */
  inline int SizeOf(Isis::PixelType pixelType) {
    if (pixelType == Isis::None) return 0;
    if (pixelType == Isis::UnsignedByte) return sizeof(unsigned char);
    if (pixelType == Isis::SignedByte) return sizeof(char);
    if (pixelType == Isis::UnsignedWord) return sizeof(unsigned short);
    if (pixelType == Isis::SignedWord) return sizeof(short);
    if (pixelType == Isis::UnsignedInteger) return sizeof(unsigned int);
    if (pixelType == Isis::SignedInteger) return sizeof(int);
    if (pixelType == Isis::Real) return sizeof(float);
    if (pixelType == Isis::Double) return sizeof(double);
    return -1;
  }

 /**
  * Returns string name of PixelType enumeration entered as input parameter
  * 
  * @param pixelType PixelType enumeration
  * 
  * @return string Name of PixelType
  */
  inline std::string PixelTypeName (Isis::PixelType pixelType) {
    if (pixelType == Isis::None) return "None";
    if (pixelType == Isis::UnsignedByte) return "UnsignedByte";
    if (pixelType == Isis::SignedByte) return "SignedByte";
    if (pixelType == Isis::UnsignedWord) return "UnsignedWord";
    if (pixelType == Isis::SignedWord) return "SignedWord";
    if (pixelType == Isis::UnsignedInteger) return "UnsignedInteger";
    if (pixelType == Isis::SignedInteger) return "SignedInteger";
    if (pixelType == Isis::Real) return "Real";
    if (pixelType == Isis::Double) return "Double";
    return "Error";
  }

 /**
  * Returns PixelType enumeration given a string 
  * 
  * @param type iString containing the name of pixel type. Acceptable values are
  *             UnsignedByte, SignedByte, UnsignedWord, SignedWord, 
  *             UnsignedInteger, SignedInteger, Read, and Double (not case 
  *             sensitive)
  * 
  * @return Isis::PixelType
  */
  inline Isis::PixelType PixelTypeEnumeration(const std::string &type) {
    Isis::iString temp(type);
    temp = temp.UpCase();
    if (temp == "UNSIGNEDBYTE" || temp == "8BIT" || temp == "8-BIT") return Isis::UnsignedByte;
    if (temp == "SIGNEDBYTE") return Isis::SignedByte;
    if (temp == "UNSIGNEDWORD") return Isis::UnsignedWord;
    if (temp == "SIGNEDWORD") return Isis::SignedWord;
    if (temp == "UNSIGNEDINTEGER") return Isis::UnsignedInteger;
    if (temp == "SIGNEDINTEGER") return Isis::SignedInteger;
    if (temp == "REAL") return Isis::Real;
    if (temp == "DOUBLE") return Isis::Double;
    return Isis::None;
  }
}

#endif

