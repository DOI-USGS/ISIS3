/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */ 
#include "GradientOperator.h"
#include "Chip.h"

namespace Isis {
  /**
   * This method returns the amount of interest for the given chip.
   *
   * @param chip
   *
   * @return the amount of interest for this chip
   */
  double GradientOperator::Interest(Chip &chip) {
    double gradient = 0.0;
    double pix1, pix2;
    int height = chip.Lines();
    int width = chip.Samples();
    int offset = 0;

    while(width > 1 && height > 1) {
      for(int i = 1; i <= width; i++) {
        pix1 = chip.GetValue(i + offset, 1 + offset);
        pix2 = chip.GetValue(width - i + 1 + offset, height + offset);
        if(ValidDnValue(pix1) && ValidDnValue(pix2)) gradient += std::abs(pix1 - pix2);
      }

      for(int i = 2; i < height; i++) {
        pix1 = chip.GetValue(1 + offset, i + offset);
        pix2 = chip.GetValue(width + offset, height - i + 1 + offset);
        if(ValidDnValue(pix1) && ValidDnValue(pix2)) gradient += std::abs(pix1 - pix2);
      }

      width -= 2;
      height -= 2;
      offset += 1;
    }

    return gradient;
  }
}

extern "C" Isis::InterestOperator *GradientOperatorPlugin(Isis::Pvl &pPvl) {
  return new Isis::GradientOperator(pPvl);
}

