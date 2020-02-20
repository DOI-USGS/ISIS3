#include "Isis.h"
#include "cnetpvl2bin.h"
#include "Application.h"
#include "Progress.h"

using namespace Isis;

void IsisMain() {
  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  Progress progress;
  cnetpvl2bin(ui, &progress);
}
