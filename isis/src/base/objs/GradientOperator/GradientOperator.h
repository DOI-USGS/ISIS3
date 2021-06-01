#ifndef GradientOperator_h
#define GradientOperator_h
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
   * @brief Gradient interest operator
   *
   * This class is used to construct a gradient interest operator.
   * For this class, the interest is always positive with the worst
   * interest amount being 0. The higher the interest, the better.
   *
   * @see InterestOperator
   *
   * @author 2006-02-11 Jacob Danton
   *
   * @internal
   *   @history 2010-06-10 Sharmila Prasad  Updated to accomadate CnetValidMeasure base class
   *   @history 2010-11-10 Sharmila Prasad - Updated unittest to accomodate changes in the DefFile
   */
  class GradientOperator : public InterestOperator {
    public:
      GradientOperator(Pvl &pPvl) : InterestOperator(pPvl) {};
      virtual ~GradientOperator() {};

    protected:
      virtual double Interest(Chip &chip);
  };
};

#endif
