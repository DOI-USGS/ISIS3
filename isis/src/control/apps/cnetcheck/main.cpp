#include "Isis.h"

#include "cnetcheck.h"

#include "Application.h"
#include "IException.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;

  try {
    QString results = cnetcheck(ui, &appLog);
    
    if (ui.IsInteractive()){
      Application::GuiLog(results);
    }
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
