#ifndef cnetbin2pvl_h
#define cnetbin2pvl_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "UserInterface.h"
#include "ControlNet.h"
#include "Progress.h"

namespace Isis {
  extern void cnetbin2pvl(UserInterface &ui, Progress *progress=0);
  extern void cnetbin2pvl(ControlNet &net, UserInterface &ui, Progress *progress=0);
}

#endif
