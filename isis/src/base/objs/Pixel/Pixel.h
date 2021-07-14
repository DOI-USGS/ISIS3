#ifndef Pixel_h
#define Pixel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SpecialPixel.h"

namespace Isis {
  /**
   * @brief Store and/or manipulate pixel values
   *
   * This class can store pixel information and also contains
   * utility methods for testing and modifying pixel and special
   * pixel values that can be used without instanteating the
   * class.
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
   *
   *  @history 2015-05-11 Kristin Berry - Added ability to store
   *                                 pixel information/made class
   *                                 instantiatable and updated
   *                                 funtionality to use this
   *                                 information when available.
   *  @history 2015-08-05 Kristin Berry - Added empty constructor, copy constructor, copy assignement
   *                          operator, and virutal destructor. Also updated to comply with Isiscoding
   *                          standards.
   *  @history 2016-04-20 Makayla Shepherd - Added methods to handle UnsignedWord pixel type.
   */
  class Pixel {
    public:
      Pixel();
      Pixel(int sample, int line, int band, double DN);
      Pixel(const Pixel& pixel);
      virtual ~Pixel();


      Pixel &operator=(const Pixel& other);

      int line() const;
      int sample() const;
      int band() const;
      double DN() const;

      static unsigned char To8Bit(const double d);
      unsigned char To8Bit();
      static short int To16Bit(const double d);
      short int To16Bit();
      static short unsigned int To16UBit(const double d);
      short unsigned int To16Ubit();
      static float To32Bit(const double d);
      float To32Bit();

      static double ToDouble(const unsigned char t);
      static double ToDouble(const short int t);
      static double ToDouble(const short unsigned int t);
      static double ToDouble(const float t);
      double ToDouble();

      static float ToFloat(const unsigned char d);
      static float ToFloat(const short int d);
      static float ToFloat(const short unsigned int d);
      static float ToFloat(const double d);
      float ToFloat();

      static std::string ToString(double d);
      std::string ToString();

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
       * use in computations.
       *
       * @return bool
       */
      bool IsSpecial() {
        return IsSpecial(m_DN);
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
       * Returns true if the input pixel is valid.  Valid implies the
       * pixel is neither hrs, lrs, his, lis, nor null.
       *
       * @return bool
       */
      bool IsValid() {
        return IsValid(m_DN);
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
       * Returns true if the input pixel is null
       *
       * @return bool
       */
      bool IsNull() {
        return IsNull(m_DN);
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
       * Returns true if the input pixel is one of the high saturation types
       *
       * @return bool
       */
      bool IsHigh() {
        return IsHigh(m_DN);
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
       * Returns true if the input pixel is one of the low saturation types
       *
       * @return bool
       */
      bool IsLow() {
        return IsLow(m_DN);
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
       * Returns true if the input pixel is high representation saturation
       *
       * @return bool
       */
      bool IsHrs() {
        return IsHrs(m_DN);
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
       * Returns true if the input pixel is high instrument saturation
       *
       * @return bool
       */
      bool IsHis() {
        return IsHis(m_DN);
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
       * Returns true if the input pixel is low instrument saturation
       *
       * @return bool
       */
      bool IsLis() {
        return IsLis(m_DN);
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

     /**
       * Returns true if the input pixel is low representation saturation
       *
       * @return bool
       */
      bool IsLrs() {
        return IsLrs(m_DN);
      }

  private:
    //! line coordinate of pixel
    int m_line;

    //! sample coordinate of pixel
    int m_sample;

    //! band coordinate of pixel
    int m_band;

    //! DN of pixel
    double m_DN;

  }; // end of pixel class
}
#endif
