/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:08 $                                                                 
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
#include "Pixel.h"

using namespace std;

namespace Isis {
  /**
   * Converts double pixels to unsigned char pixels with special pixel
   * translations
   *
   * @param d Double pixel value to be converted to a double
   *
   * @return unsigned char The unsigned char pixel value
   */
  unsigned char Pixel::To8Bit(const double d) {
    if (d < VALID_MIN8) {
      if (d == NULL8)                return NULL1;
      else if (d == LOW_INSTR_SAT8)  return LOW_INSTR_SAT1;
      else if (d == LOW_REPR_SAT8)   return LOW_REPR_SAT1;
      else if (d == HIGH_INSTR_SAT8) return HIGH_INSTR_SAT1;
      else if (d == HIGH_REPR_SAT8)  return HIGH_REPR_SAT1;
      else                           return LOW_REPR_SAT1;
    }
    else {
      if (d < VALID_MIN1 - 0.5)      return LOW_REPR_SAT1;
      else if (d > VALID_MAX1 + 0.5) return HIGH_REPR_SAT1;

      else {
        int itemp = (int) (d + 0.5);
        if (itemp < VALID_MIN1)      return LOW_REPR_SAT1;
        else if (itemp > VALID_MAX1) return HIGH_REPR_SAT1;
        else                         return (unsigned char) (d + 0.5);
      }
    }
  }

  /**
   * Converts double pixels to short int pixels with special pixel translations
   *
   * @param d Double pixel value to be converted to a double
   *
   * @return short int The short int pixel value
   */
  short int Pixel::To16Bit(const double d) {
    if (d < VALID_MIN8) {
      if (d == NULL8)                return NULL2;
      else if (d == LOW_INSTR_SAT8)  return LOW_INSTR_SAT2;
      else if (d == LOW_REPR_SAT8)   return LOW_REPR_SAT2;
      else if (d == HIGH_INSTR_SAT8) return HIGH_INSTR_SAT2;
      else if (d == HIGH_REPR_SAT8)  return HIGH_REPR_SAT2;
      else                           return LOW_REPR_SAT2;
    }
    else {
      if (d < VALID_MIN2 - 0.5)      return LOW_REPR_SAT2;
      else if (d > VALID_MAX2 + 0.5) return HIGH_REPR_SAT2;

      else {
        int itemp;
        if (d < 0.0) {
          itemp = (int) (d - 0.5);
        }
        else {
          itemp = (int) (d + 0.5);
        }

        if (itemp < VALID_MIN2)      return LOW_REPR_SAT2;
        else if (itemp > VALID_MAX2) return HIGH_REPR_SAT2;
        else if (d < 0.0)            return (short) (d - 0.5);
        else                         return (short) (d + 0.5);
      }
    }
  }

  /**
   * Converts double pixels to float pixels with special pixel translations
   *
   * @param d Double pixel value to be converted to a double
   *
   * @return float The float pixel value
   */
  float Pixel::To32Bit(const double d) {
    if (d < (double) VALID_MIN8) {
      if (d == NULL8)                 return(NULL4);
      else if (d == LOW_REPR_SAT8)    return(LOW_REPR_SAT4);
      else if (d == LOW_INSTR_SAT8)   return(LOW_INSTR_SAT4);
      else if (d == HIGH_INSTR_SAT8)  return(HIGH_INSTR_SAT4);
      else if (d == HIGH_REPR_SAT8)   return(HIGH_REPR_SAT4);
      else                            return(LOW_REPR_SAT4);
    }
    else if (d > (double) VALID_MAX8) return(HIGH_REPR_SAT8);
    else                              return((float) d);
  }

  /**
   * Converts unsigned char pixels to double pixels with special pixel
   * translations
   *
   * @param d Unsigned char pixel value to be converted to a double
   *
   * @return double The double pixel value
   */
  double Pixel::ToDouble(const unsigned char d) {
    if (d < VALID_MIN1) {
      if (d == NULL1)                return(NULL8);
      else if (d == LOW_REPR_SAT1)   return(LOW_REPR_SAT8);
      else if (d == LOW_INSTR_SAT1)  return(LOW_INSTR_SAT8);
      else if (d == HIGH_REPR_SAT1)  return(HIGH_REPR_SAT8);
      else if (d == HIGH_INSTR_SAT1) return(HIGH_INSTR_SAT8);
      else                           return(LOW_REPR_SAT8);
    }
    else if (d > VALID_MAX1)         return(HIGH_REPR_SAT8);
    else                             return((double) d);
  }

  /**
   * Converts short int pixels to double pixels with special pixel translations
   *
   * @param d Short int pixel value to be converted to a double
   *
   * @return double The double pixel value
   */
  double Pixel::ToDouble(const short int d) {
    if (d < VALID_MIN2) {
      if (d == NULL2)                 return(NULL8);
      else if (d == LOW_REPR_SAT2)    return(LOW_REPR_SAT8);
      else if (d == LOW_INSTR_SAT2)   return(LOW_INSTR_SAT8);
      else if (d == HIGH_REPR_SAT2)   return(HIGH_REPR_SAT8);
      else if (d == HIGH_INSTR_SAT2)  return(HIGH_INSTR_SAT8);
      else                            return(LOW_REPR_SAT8);
    }
    else                              return((double) d);
  }

  /**
   * Converts float pixels to double pixels with special pixel translations
   *
   * @param d Float pixel value to be converted to a double
   *
   * @return double The double pixel value
   */
  double Pixel::ToDouble(const float d) {
    if (d < VALID_MIN4) {
      if (d == NULL4)                return(NULL8);
      else if (d == LOW_REPR_SAT4)   return(LOW_REPR_SAT8);
      else if (d == LOW_INSTR_SAT4)  return(LOW_INSTR_SAT8);
      else if (d == HIGH_REPR_SAT4)  return(HIGH_REPR_SAT8);
      else if (d == HIGH_INSTR_SAT4) return(HIGH_INSTR_SAT8);
      else                           return(LOW_REPR_SAT8);
    }
    else if (d > VALID_MAX4)         return(HIGH_REPR_SAT8);
    else                             return((double) d);
  }

  /**
   * Converts unsigned char to float with pixel translations
   * and care for overflows (underflows are assumed to cast to
   * 0!)
   *
   * @param t Unsigned char pixel value to be converted to a
   *          float
   *
   * @return float The float pixel value
   */
  float Pixel::ToFloat(const unsigned char t) {
    if (t < (double) VALID_MIN1) {
      if (t == NULL1)                 return(NULL4);
      else if (t == LOW_REPR_SAT1)    return(LOW_REPR_SAT4);
      else if (t == LOW_INSTR_SAT1)   return(LOW_INSTR_SAT4);
      else if (t == HIGH_INSTR_SAT1)  return(HIGH_INSTR_SAT4);
      else if (t == HIGH_REPR_SAT1)   return(HIGH_REPR_SAT4);
      else                            return(LOW_REPR_SAT4);
    }
    else if (t > (double) VALID_MAX1) return(HIGH_REPR_SAT8);
    else                              return((float) t);
  }

  /**
   * Converts short int to float with pixel translations and
   * care for overflows (underflows are assumed to cast to 0!)
   *
   * @param t Short int pixel value to be converted to a float
   *
   * @return float The float pixel value
   */
  float Pixel::ToFloat(const short int t) {
    if (t < (double) VALID_MIN2) {
      if (t == NULL2)                 return(NULL4);
      else if (t == LOW_REPR_SAT2)    return(LOW_REPR_SAT4);
      else if (t == LOW_INSTR_SAT2)   return(LOW_INSTR_SAT4);
      else if (t == HIGH_INSTR_SAT2)  return(HIGH_INSTR_SAT4);
      else if (t == HIGH_REPR_SAT2)   return(HIGH_REPR_SAT4);
      else                            return(LOW_REPR_SAT4);
    }
    else if (t > (double) VALID_MAX2) return(HIGH_REPR_SAT8);
    else                              return((float) t);
  }

  /**
   * Converts double to float with pixel translations and
   * care for overflows (underflows are assumed to cast to 0!)
   *
   * @param t Double pixel value to be converted to a float
   *
   * @return float The float pixel value
   */
  float Pixel::ToFloat(const double t) {
    if (t < (double) VALID_MIN8) {
      if (t == NULL8)                 return(NULL4);
      else if (t == LOW_REPR_SAT8)    return(LOW_REPR_SAT4);
      else if (t == LOW_INSTR_SAT8)   return(LOW_INSTR_SAT4);
      else if (t == HIGH_INSTR_SAT8)  return(HIGH_INSTR_SAT4);
      else if (t == HIGH_REPR_SAT8)   return(HIGH_REPR_SAT4);
      else                            return(LOW_REPR_SAT4);
    }
    else if (t > (double) VALID_MAX8) return(HIGH_REPR_SAT8);
    else                              return((float) t);
  }

  /**
   * Takes a double pixel value and returns the name of the pixel type as a
   * string
   *
   * @param d Pixel value
   *
   * @return string The name of the pixel type
   */
  string Pixel::ToString(double d) {
    if (IsSpecial(d)) {
      if (IsNull(d))     return string("Null");
      else if (IsLrs(d)) return string("Lrs");
      else if (IsHrs(d)) return string("Hrs");
      else if (IsHis(d)) return string("His");
      else if (IsLis(d)) return string("Lis");
      else               return string("Invalid");
    }

    QString result;
    return result.setNum(d).toStdString();
  }
}
