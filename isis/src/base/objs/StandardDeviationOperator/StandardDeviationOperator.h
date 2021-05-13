#ifndef StandardDeviationOperator_h
#define StandardDeviationOperator_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "InterestOperator.h"

namespace Isis {
  class Pvl;
  class Chip;

  /**
   * @brief Standard deviation interest operator
   *
   * This class is used to construct a standard deviation interest operator.
   * For this class, the interest is always positive with the worst
   * interest amount being 0. The higher the interest, the better.
   *
   * @see InterestOperator
   *
   * @author 2006-02-11 Jacob Danton
   *
   * @internal
   *   @history 2006-02-11 Jacob Danton - Original Version
   *   @history 2007-08-01 Steven Koechle - Fixed error where
   *            Interest was compairing the uninitialized value of
   *            pixels[n] to see if it was a special pixel.
   *   @history 2007-08-02 Steven Koechle - Removed
    *            CompareInterests virtual method.
   *    @history 2010-06-10 Sharmila Prasad - Changes to accomodate CnetValidMeasure base class
   *    @history 2010-11-10 Sharmila Prasad - Updated unittest for changes in the deffile
   */
  class StandardDeviationOperator : public InterestOperator {
    public:
      StandardDeviationOperator(Pvl &pPvl) : InterestOperator(pPvl) {
        p_worstInterest = 0.0;
      };
      virtual ~StandardDeviationOperator() {};

    protected:
      virtual double Interest(Chip &chip);
  };
};

#endif
