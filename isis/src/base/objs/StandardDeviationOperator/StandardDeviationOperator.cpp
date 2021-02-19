/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "StandardDeviationOperator.h"
#include "Chip.h"
#include "Statistics.h"

namespace Isis {
  /**
   * This method returns the amount of interest for the given chip.
   *
   * @param chip
   *
   * @return the amount of interest for this chip
   */
  double StandardDeviationOperator::Interest(Chip &chip) {
    Statistics stats;
    stats.SetValidRange(mdMinDN, mdMaxDN);
    for(int i = 0; i < chip.Samples(); i++) {
      double pixels[chip.Lines()];
      int n = 0;
      for(int j = 0; j < chip.Lines(); j++) {
        if(!IsSpecial(chip.GetValue(i + 1, j + 1))) {
          pixels[n] = chip.GetValue(i + 1, j + 1);
          n++;
        }
      }
      stats.AddData(pixels, n);
    }

    return stats.StandardDeviation();
  }
}

extern "C" Isis::InterestOperator *StandardDeviationOperatorPlugin(Isis::Pvl &pPvl) {
  return new Isis::StandardDeviationOperator(pPvl);
}

