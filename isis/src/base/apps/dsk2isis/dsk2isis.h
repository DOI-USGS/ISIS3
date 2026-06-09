/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef dsk2isis_h
#define dsk2isis_h

#include "UserInterface.h"

namespace Isis {
  /**
   * Convert a NAIF DSK shape model to an ISIS cube.
   *
   * @param ui UserInterface containing FROM, MAP, TO, and METHOD parameters.
   */
  extern void dsk2isis(UserInterface &ui);
}

#endif
