/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef equalizer_h
#define equalizer_h

#include "UserInterface.h"

namespace Isis {
  /**
   * @brief Tone matches map projected cubes for mosaicking
   *
   * This function performs brightness and/or contrast equalization on a list
   * of overlapping map-projected cubes. It calculates multiplicative (GAIN) and
   * additive (OFFSET) corrections using a least squares solution based on
   * overlap statistics.
   *
   * @param ui UserInterface containing parameters for equalization
   *
   * @throws IException::User If there are insufficient overlaps or images without overlaps
   */
  extern void equalizer(UserInterface &ui);
}

#endif
