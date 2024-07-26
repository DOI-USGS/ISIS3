/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef phocube_h
#define phocube_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void phocube(Cube *icube, UserInterface &ui);
  extern void phocube(UserInterface &ui);
}

#endif
