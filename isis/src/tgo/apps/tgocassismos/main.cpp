#include "Isis.h"

#include "Application.h"
#include "tgocassismos.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  tgocassismos(ui);
}