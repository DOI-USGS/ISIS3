/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "NoOperator.h"
#include "Chip.h"
#include "Statistics.h"

namespace Isis {
  /**
   * This method returns a constant so all points have equal interest
   *
   * @param chip
   *
   * @return pi * e
   */
  double NoOperator::Interest(Chip &chip) {
    // Important: Interest = pi * e
    return Isis::PI * Isis::E;
  }
}

extern "C" Isis::InterestOperator *NoOperatorPlugin(Isis::Pvl &pvl) {
  return new Isis::NoOperator(pvl);
}

