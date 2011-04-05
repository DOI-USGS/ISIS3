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
  cnet.ReadControl(ui.GetFilename("FROM"), &p);
  p.SetText("Writing Control Network...");
  p.SetMaximumSteps(1);
  p.CheckStatus();
  cnet.WritePvl(ui.GetFilename("TO"));
  p.CheckStatus();
}
