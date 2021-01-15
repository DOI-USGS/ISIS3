#include "Isis.h"

#include "Application.h"
#include "Process.h"
#include "UserInterface.h"

#include "pds2isis.h"

using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    pds2isis(ui, &appLog);
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
