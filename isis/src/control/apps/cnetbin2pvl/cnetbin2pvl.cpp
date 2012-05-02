#include "Isis.h"

#include "ControlNet.h"
#include "iString.h"
#include "Progress.h"

using namespace Isis;

void IsisMain() {
  // Get user entered file name & mode 
  UserInterface &ui = Application::GetUserInterface();
  Progress p;

  ControlNet cnet;
  cnet.ReadControl(ui.GetFileName("FROM"), &p);
  p.SetText("Writing Control Network...");
  p.SetMaximumSteps(1);
  p.CheckStatus();
  cnet.Write(ui.GetFileName("TO"), true);
  p.CheckStatus();
}
