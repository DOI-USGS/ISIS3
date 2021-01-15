#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "reduce_app.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    reduce(ui, &appLog);
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
