#include "Isis.h"
#include "Pvl.h"
#include "OriginalLabel.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFilename("FROM");

  // Extract history from file
  OriginalLabel origLab(file);
  Pvl pvl = origLab.ReturnLabels();
  if (ui.IsInteractive()) {
    Application::GuiLog(pvl);
  }
  else if (ui.WasEntered("TO")) {
    if (ui.GetBoolean("APPEND")) {
      pvl.Append(Filename(ui.GetFilename("TO")).Expanded());
    }
    else {
      pvl.Write(Filename(ui.GetFilename("TO")).Expanded());
    }
  } 
  else {
    cout << pvl << endl;
  }
}
