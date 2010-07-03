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
  double NoOperator::Interest (Chip &chip) {
    // Important: Interest = pi * e
    return Isis::PI * Isis::E;
  }
}

extern "C" Isis::InterestOperator *NoOperatorPlugin (Isis::Pvl &pvl) {
  return new Isis::NoOperator(pvl);
}

