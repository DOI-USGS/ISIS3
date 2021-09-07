#include "Isis.h"

#include "Application.h"
#include "tgocassisrdrgen.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  tgocassisrdrgen(ui);
}