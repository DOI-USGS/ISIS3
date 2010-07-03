#ifndef Pixel_h
#define Pixel_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2009/02/03 23:14:47 $
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
  #include "SpecialPixel.h"

namespace Isis {
  /**
   * @brief Manipulate pixel values
   * 
   * This class contains utility methods for testing and modifying pixel and
   * special pixel values.
   *
   * @ingroup Utility
   *
   * @author 2002-04-11 Kris Becker
   *
   * @internal
   *  @history 2003-02-11 Jeff Anderson - Wrote unitTest and documentation
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2005-05-18 Jeff Anderson - Changed long to int for 64-bit port
   *  @history 2006-06-21 Elizabeth Miller - Copied SpecialPixel methods into
   *                                         into the Pixel class for easy
   *                                         refactoring later and added
   *                                         several conversion methods
   *  @history 2009-02-03 Travis Addair - Modified documentation
   *           for clarity
   */
  class Pixel {
  public:
    static unsigned char To8Bit(const double d);
    static short int To16Bit(const double d);
    static float To32Bit(const double d);

    static double ToDouble(const unsigned char t);
    static double ToDouble(const short int t);
    static double ToDouble(const float t);

    static float ToFloat(const unsigned char d);
    static float ToFloat(const short int d);
    static float ToFloat(const double d);

    static std::string ToString(double d);

    /**
     * Returns true if the input pixel is special. Not special implies it is valid to
     * use in computations.
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsSpecial(const double d) {
      return(d < VALID_MIN8);
    }

    /**
     * Returns true if the input pixel is special. Not special implies it is valid to
     * use in computations. This method applies to a 4-byte floating point rather
     * than an 8-byte double.
     *
     * @param f Pixel value to test
     *
     * @return bool
     */
    static inline bool IsSpecial(const float f) {
      return(f < VALID_MIN4);
    }

    /**
     * Returns true if the input pixel is valid.  Valid implies the 
     * pixel is neither hrs, lrs, his, lis, nor null. 
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsValid(const double d) {
      return(d >= VALID_MIN8);
    }

    /**
     * Returns true if the input pixel is null
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsNull(const double d) {
      return(d == NULL8);
    }

    /**
     * Returns true if the input pixel is one of the high saturation types
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsHigh(const double d) {
      return(d == HIGH_REPR_SAT8) || (d == HIGH_INSTR_SAT8);
    }

    /**
     * Returns true if the input pixel is one of the low saturation types
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsLow(const double d) {
      return(d == LOW_REPR_SAT8) || (d == LOW_INSTR_SAT8);
    }

    /**
     * Returns true if the input pixel is high representation saturation
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsHrs(const double d) {
      return(d == HIGH_REPR_SAT8);
    }

    /**
     * Returns true if the input pixel is high instrument saturation
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsHis(const double d) {
      return(d == HIGH_INSTR_SAT8);
    }

    /**
     * Returns true if the input pixel is low instrument saturation
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsLis(const double d) {
      return(d == LOW_INSTR_SAT8);
    }

    /**
     * Returns true if the input pixel is low representation saturation
     *
     * @param d Pixel value to test
     *
     * @return bool
     */
    static inline bool IsLrs(const double d) {
      return(d == LOW_REPR_SAT8);
    }

  }; // end of pixel class
}
#endif
