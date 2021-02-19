/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "MoravecOperator.h"
#include "Chip.h"

namespace Isis {
  /**
   * This method returns the amount of interest for the given chip.
   *
   * @param chip The chip to calculate the interest value of
   *
   * @return the amount of interest for this chip
   */
  double MoravecOperator::Interest(Chip &chip) {
    int height = chip.Lines();
    int width = chip.Samples();
    std::vector<double> interests;
    double smallestInterest = 0.0;

    // Create offsets for comparison of different areas of the chip
    for(int offX = -1; offX <= 1; offX++) {
      for(int offY = -1; offY <= 1; offY++) {
        // doesnt do work if offset would be center chip
        if(offX == 0 && offY == 0) continue;
        // Do interest computation between center and offset areas and store into array
        double interest = 0.0;
        for(int y = 2; y <= height - 1; y++) {
          for(int x = 2; x <= width - 1; x++) {
            // If either pixel is special ignore difference
            if(ValidDnValue(chip.GetValue(x, y)) && ValidDnValue(chip.GetValue(x + offX, y + offY))) {
              interest += std::pow(chip.GetValue(x, y) - chip.GetValue(x + offX, y + offY), 2);
            }
          }
        }
        // Initialize smallest interest with first value
        if(interests.size() == 1) {
          smallestInterest = interest;
        }
        interests.push_back(interest);
      }
    }
    // find smallest interest and return
    for(unsigned int i = 0; i < interests.size(); i++) {
      if(interests[i] < smallestInterest) {
        smallestInterest = interests[i];
      }
    }
    return smallestInterest;
  }

  /**
  * Sets an offset to pass in larger chips if operator requires it
  * This is used to offset the subchip size passed into Interest
  *
  * @return int   Amount to add to both x & y total sizes
  */
  int MoravecOperator::Padding() {
    return 2;
  }
}
extern "C" Isis::InterestOperator *MoravecOperatorPlugin(Isis::Pvl &pPvl) {
  return new Isis::MoravecOperator(pPvl);
}
