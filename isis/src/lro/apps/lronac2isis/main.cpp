#include "Isis.h"

#include "Application.h"
#include "lronac2isis.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  lronac2isis(ui);
}