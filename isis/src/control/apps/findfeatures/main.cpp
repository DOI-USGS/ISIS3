#include "Isis.h"

#include "Application.h"
#include "UserInterface.h"
#include "Pvl.h"
#include "SessionLog.h"
#include "findfeatures.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    findfeatures(ui, &appLog);
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

  PvlGroup results = appLog.findGroup("Results");
  if( ui.WasEntered("TO") && ui.IsInteractive() ) {
    Application::GuiLog(results);
  }

  SessionLog::TheLog().AddResults(results);
}
