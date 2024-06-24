#ifndef cnetthinner_h
#define cnetthinner_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "UserInterface.h"
#include "ControlNet.h"
#include "Pvl.h"

namespace Isis {
  extern Pvl cnetthinner(UserInterface &ui);
  extern Pvl cnetthinner(QSharedPointer<ControlNet> &cnet, UserInterface &ui);
}

#endif
