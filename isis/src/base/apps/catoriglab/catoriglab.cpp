#include "Isis.h"
#include "Pvl.h"
#include "OriginalLabel.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  QString file = ui.GetFileName("FROM");

  // Extract history from file
  OriginalLabel origLab(file);
  Pvl pvl = origLab.ReturnLabels();
  if (ui.IsInteractive()) {
    Application::GuiLog(pvl);
  }
  else if (ui.WasEntered("TO")) {
    if (ui.GetBoolean("APPEND")) {
      pvl.Append(FileName(ui.GetFileName("TO")).expanded());
    }
    else {
      pvl.Write(FileName(ui.GetFileName("TO")).expanded());
    }
  } 
  else {
    cout << pvl << endl;
  }
}
