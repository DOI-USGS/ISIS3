#ifndef ForstnerOperator_h
#define ForstnerOperator_h
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
   * @brief Forstner interest operator
   *
   * This class is used to construct a forstner interest operator.
   * For this class, the interest is always positive with the worst
   * interest amount being 0. The higher the interest, the better.
   *
   * @see InterestOperator
   *
   * @see "A Fast Operator for Detection and Precise Location of
   *      Distinct Points, Corners and Centres of Circular
   *      Features" by W. Forstner and E. Gulch    (Forstner.pdf)
   *
   * @author 2006-05-01 Jacob Danton
   *
   * @internal
   *   @history 2010-11-10 Sharmila Prasad - Updated unittest for changes in the
   *                           deffile 
   */
  class ForstnerOperator : public InterestOperator {
    public:
      ForstnerOperator(Pvl &pvl) : InterestOperator(pvl) {};
      virtual ~ForstnerOperator() {};

    protected:
      virtual double Interest(Chip &chip);
  };
};

#endif
