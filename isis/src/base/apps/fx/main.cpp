#include "Isis.h"
#include "fx.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  fx(ui);
}

