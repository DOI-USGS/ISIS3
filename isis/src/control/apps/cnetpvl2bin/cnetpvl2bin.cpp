#include "Isis.h"

#include "ControlNet.h"
#include "Progress.h"

using namespace Isis;

void IsisMain() {
  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  Progress p;

  ControlNet cnet;
  cnet.ReadControl(ui.GetFilename("FROM"), &p);
  cnet.Write(ui.GetFilename("TO"));
}
