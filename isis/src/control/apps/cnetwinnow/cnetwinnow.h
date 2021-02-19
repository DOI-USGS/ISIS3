#ifndef cnetwinnow_h
#define cnetwinnow_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ControlNet.h"
#include "ControlMeasure.h"
#include "UserInterface.h"
#include "Progress.h"

namespace Isis {
  extern void cnetwinnow(UserInterface &ui, Progress *progress = 0);

  extern void cnetwinnow(ControlNet &net, SerialNumberList &serialNumList, UserInterface &ui, Progress *progress = 0);
}

#endif
