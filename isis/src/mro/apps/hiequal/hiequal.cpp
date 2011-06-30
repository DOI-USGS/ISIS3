#include "Isis.h"

#include "HiEqualization.h"

using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Make sure the user enters an "OUTSTATS" file if the CALCULATE option
  // is selected
  std::string processOpt = ui.GetString("PROCESS");
  if (processOpt == "CALCULATE") {
    if (!ui.WasEntered("OUTSTATS")) {
      std::string msg = "If the CALCULATE option is selected, you must enter";
      msg += " an OUTSTATS file";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  HiEqualization equalizer(ui.GetFilename("FROMLIST"));

  // Read hold list if one was entered
  if (ui.WasEntered("HOLD")) {
    equalizer.addHolds(ui.GetFilename("HOLD"));
  }

  if (processOpt != "APPLY") {
    equalizer.calculateStatistics();

    // Write the results to the log
    PvlGroup results = equalizer.getResults();
    Application::Log(results);

    // Setup the output text file if the user requested one
    if (ui.WasEntered("OUTSTATS")) {
      equalizer.write(ui.GetFilename("OUTSTATS"));
    }
  }
  else {
    equalizer.importStatistics(ui.GetFilename("INSTATS"));
  }

  // Apply the correction to the images if the user wants this done
  if (processOpt != "CALCULATE") {
    equalizer.applyCorrection(ui.WasEntered("TOLIST") ?
        ui.GetFilename("TOLIST") : "");
  }
}

