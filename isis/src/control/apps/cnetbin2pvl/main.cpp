#include "Isis.h"

#include "cnetbin2pvl.h"
#include "Application.h"

using namespace Isis;

void IsisMain() {
  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  cnetbin2pvl(ui);
}
