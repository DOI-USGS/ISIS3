/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include "HiEqualization.h"

using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Make sure the user enters an "OUTSTATS" file if the CALCULATE option
  // is selected
  QString processOpt = ui.GetString("PROCESS");
  if (processOpt == "CALCULATE") {
    if (!ui.WasEntered("OUTSTATS")) {
      std::string msg = "If the CALCULATE option is selected, you must enter";
      msg += " an OUTSTATS file";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  HiEqualization equalizer(ui.GetFileName("FROMLIST"));

  // Read hold list if one was entered
  if (ui.WasEntered("HOLD")) {
    equalizer.addHolds(ui.GetFileName("HOLD"));
  }

  if (processOpt != "APPLY") {
    equalizer.calculateStatistics();

    // Write the results to the log
    PvlGroup results = equalizer.getResults();
    Application::Log(results);

    // Setup the output text file if the user requested one
    if (ui.WasEntered("OUTSTATS")) {
      equalizer.write(ui.GetFileName("OUTSTATS"));
    }
  }
  else {
    equalizer.importStatistics(ui.GetFileName("INSTATS"));
  }

  // Apply the correction to the images if the user wants this done
  if (processOpt != "CALCULATE") {
    equalizer.applyCorrection(ui.WasEntered("TOLIST") ?
        ui.GetFileName("TOLIST") : "");
  }
}
