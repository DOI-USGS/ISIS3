#ifndef NoOperator_h
#define NoOperator_h
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
   * @brief no interest operator
   *
   * This class is used to construct a no interest operator.
   * For this class, the interest returned is the constant pi * e
   *
   * @see InterestOperator
   *
   * @author 2008-12-12 Christopher Austin
   *
   * @internal
   *   @history 2008-12-12 Jacob Danton - Original Version
   *   @history 2010-11-10 Sharmila Prasad - Updated unittest for changes in the deffile
   */
  class NoOperator : public InterestOperator {
    public:
      NoOperator(Pvl &pvl) : InterestOperator(pvl) {
        p_worstInterest = 0.0;
      };
      virtual ~NoOperator() {};

    protected:
      virtual double Interest(Chip &chip);
  };
};

#endif
