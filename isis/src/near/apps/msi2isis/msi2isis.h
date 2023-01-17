#ifndef msi2isis_h
#define msi2isis_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  extern void msi2isis( UserInterface &ui, Pvl *log );
}

#endif
