#ifndef autoseed_h
#define autoseed_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ControlNet.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  enum SeedDomain {
    XY,
    SampleLine
  };

  extern void autoseed(UserInterface &ui,
                       Pvl *log = nullptr);
  extern void autoseed(UserInterface &ui, SerialNumberList &serialNumbers, ControlNet *precnet = nullptr, Pvl *log=nullptr);
}

#endif
