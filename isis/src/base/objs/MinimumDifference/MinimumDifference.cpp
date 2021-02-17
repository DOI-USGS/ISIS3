/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "MinimumDifference.h"
#include "Chip.h"

namespace Isis {

  /**
   * @brief Minimum difference match algorithm
   *
   * This virtual function overrides the pure virtual method in the AutoReg
   * class.  In this case, we sum the absolute value of the differences between
   * pixels in the pattern and subsearch chips and divide by the valid pixel
   * count. We ignore special pixels
   *
   * @param pattern [in] A Chip object usually containing an nxm area of a cube.
   *                     Must be the same diminsions as \b subsearch.
   * @param subsearch [in] A Chip object usually containing an nxm area of a cube.
   *                  Must be the same diminsions as \b pattern. This is normally
   *                  a subarea of a larger portion of the image.
   *
   * @return The sum of the absolute value of the DN differences divided by the
   *         valid pixel count OR Isis::NULL if the valid pixel percent is not
   *         met.
   */
  double MinimumDifference::MatchAlgorithm(Chip &pattern, Chip &subsearch) {
    // calculate the sampling information

    double diff = 0.0;
    double count = 0;
    for(double l = 1.0; l <= pattern.Lines(); l++) {
      for(double s = 1.0; s <= pattern.Samples(); s++) {
        int line = (int)l;
        int samp = (int)s;

        double pdn = pattern.GetValue(samp, line);
        double sdn = subsearch.GetValue(samp, line);
        if(IsSpecial(pdn)) continue;
        if(IsSpecial(sdn)) continue;
        diff += fabs(pdn - sdn);
        count++;
      }
    }

    return diff / count;
  }

  /**
   * This virtual method must return if the 1st fit is equal to or better
   * than the second fit.
   *
   * @param fit1  1st goodness of fit
   * @param fit2  2nd goodness of fit
   */
  bool MinimumDifference::CompareFits(double fit1, double fit2) {
    return (fit1 <= fit2);
  }
}

extern "C" Isis::AutoReg *MinimumDifferencePlugin(Isis::Pvl &pvl) {
  return new Isis::MinimumDifference(pvl);
}

