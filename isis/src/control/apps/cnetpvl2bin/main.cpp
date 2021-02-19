/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"
#include "cnetpvl2bin.h"
#include "Application.h"
#include "Progress.h"

using namespace Isis;

void IsisMain() {
  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  Progress progress;
  cnetpvl2bin(ui, &progress);
}
