#ifndef Stretch_h
#define Stretch_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include <string>
#include "Pvl.h"
#include "ImageHistogram.h"
#include "Histogram.h"

namespace Isis {
  /**
   * @brief Pixel value mapper
   *
   * This class is used to stretch or remap pixel values. For example, it can be
   * used to apply contrast stretches, color code stretches, or remap from a
   * double range to 8-bit (0 to 255). The methodology used is straightforward.
   * The program must set up a list of stretch pairs, input-to-output mappings,
   * using the AddPair method. For example, (0,0) and (1,255) are two pairs which
   * would cause an input of 0 to be mapped to 0, 0.5 would be mapped to 127.5
   * and 1 would be mapped to 255. More than two pairs can be used which
   * generates piece-wise linear mappings. Special pixels are mapped to themselves
   * unless overridden with methods such as SetNull. Input values outside the
   * minimum and maximum input pair values are mapped to LRS and HRS respectively.
   *
   * If you would like to see Stretch being used in implementation,
   * see stretch.cpp
   *
   * @ingroup Utility
   *
   * @author 2002-05-15 Jeff Anderson
   *
   * @internal
   *  @history 2002-09-17 Jeff Anderson - Added Parse method
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2005-02-16 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2005-03-11 Elizabeth Ribelin - Modified unitTest to test all
   *                                          methods in the class
   *  @history 2006-05-25 Jacob Danton - Fixed typo in documentation
   *  @history 2007-03-02 Elizabeth Miller - Added Load and Save
   *                                       methods
   *  @history 2008-11-12 Steven Lambright - Changed search algorithm into a
   *                                 binary search replacing a linear search.
   *  @history 2009-04-30 Eric Hyer - Modified parse method.  New private method
   *               NextPair() now handles low level details of parsing.
   *               Created second Parse method for handling pairs where the
   *               input side is a perentage.  Fixed Input and Output getters
   *               to check both sides of boundry condition for valid data
   *  @history 2020-02-27 Kristin Berry - Updated to inherit from Blob so Stretches can be
   *               saved and restored from cubes.
   */
  class Stretch {
    private:
      std::vector<double> p_input;   //!< Array for input side of stretch pairs
      std::vector<double> p_output;  //!< Array for output side of stretch pairs
      int p_pairs;                   //!< Number of stretch pairs

      double p_null; /**<Mapping of input NULL values go to this value
                         (default NULL)*/
      double p_lis;  /**<Mapping of input LIS values go to this value
                         (default LIS)*/
      double p_lrs;  /**<Mapping of input LRS values go to this value
                         (default LRS)*/
      double p_his;  /**<Mapping of input HIS values go to this value
                         (default HIS)*/
      double p_hrs;  /**<Mapping of input HRS values go to this value
                         (default HRS)*/
      double p_minimum; //!<By default this value is set to p_lrs
      double p_maximum; //!<By default this value is set to p_hrs

      std::pair<double, double> NextPair(QString &pairs);

    public:
      Stretch();

      //! Destroys the Stretch object
      ~Stretch() {};

      void AddPair(const double input, const double output);

      /**
       * Sets the mapping for NULL pixels. If not called the NULL pixels will be
       * mapped to NULL. Otherwise you can map NULLs to any double value.
       * For example, SetNull(0.0).
       *
       * @param value Value to map input NULLs
       */
      void SetNull(const double value) {
        p_null = value;
      }

      /**
       * Sets the mapping for LIS pixels. If not called the LIS pixels will be mapped
       * to LIS. Otherwise you can map LIS to any double value. For example,
       * SetLis(0.0).
       *
       * @param value Value to map input LIS
       */
      void SetLis(const double value) {
        p_lis = value;
      }

      /**
       * Sets the mapping for LRS pixels. If not called the LRS pixels will be mapped
       * to LRS. Otherwise you can map LRS to any double value. For example,
       * SetLrs(0.0).
       *
       * @param value Value to map input LRS
       */
      void SetLrs(const double value) {
        p_lrs = value;
      }

      /**
       * Sets the mapping for HIS pixels. If not called the HIS pixels will be mapped
       * to HIS. Otherwise you can map HIS to any double value. For example,
       * SetHis(255.0).
       *
       * @param value Value to map input HIS
       */
      void SetHis(const double value) {
        p_his = value;
      }

      /**
       * Sets the mapping for HRS pixels. If not called the HRS pixels will be mapped
       * to HRS. Otherwise you can map HRS to any double value. For example,
       * SetHrs(255.0).
       *
       * @param value Value to map input HRS
       */
      void SetHrs(const double value) {
        p_hrs = value;
      }

      void SetMinimum(const double value) {
        p_minimum = value;
      }
      void SetMaximum(const double value) {
        p_maximum = value;
      }

      void Load(Pvl &pvl, QString &grpName);
      void Save(Pvl &pvl, QString &grpName);
      void Load(QString &file, QString &grpName);
      void Save(QString &file, QString &grpName);

      double Map(const double value) const;

      void Parse(const QString &pairs);
      void Parse(const QString &pairs, const Isis::Histogram *hist);

      QString Text() const;

      //! Returns the number of stretch pairs
      int Pairs() const {
        return p_pairs;
      };

      double Input(const int index) const;
      double Output(const int index) const;

      //! Clears the stretch pairs
      void ClearPairs() {
        p_pairs = 0;
        p_input.clear();
        p_output.clear();
      };

      void CopyPairs(const Stretch &other);
  };
};

#endif
