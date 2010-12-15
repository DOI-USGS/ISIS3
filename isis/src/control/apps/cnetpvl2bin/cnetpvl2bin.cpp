#include "Isis.h"

#include <sstream>

#include "Pvl.h"
#include "ControlNet.h"
#include "iString.h"

using namespace Isis;
using namespace std;

void IsisMain(){

  // Get user entered file name & mode 
  UserInterface &ui = Application::GetUserInterface();

  ControlNet *cnet = new ControlNet;
  cnet->ReadControl(ui.GetFilename("FROM"));
  cnet->WritePB(ui.GetFilename("TO"));

  delete cnet;

}
