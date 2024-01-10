/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#ifndef noproj_h
#define noproj_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void noproj(UserInterface &ui);
  extern void noproj(Cube *icube, Cube *mcube, UserInterface &ui);
}

#endif
