/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// $Id: hyb2onccal.cpp 6045 2015-02-07 02:06:59Z moses@GS.DOI.NET $
#include "Isis.h"

#include "UserInterface.h"
#include "hyb2onccal.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    hyb2onccal(ui, &appLog);
  }
  catch (...) {
    for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
      Application::Log(*grpIt);
    }
    throw;
  }

  for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
    Application::Log(*grpIt);
  }
}
