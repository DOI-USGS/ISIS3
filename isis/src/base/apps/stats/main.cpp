#include "Isis.h"

#include "UserInterface.h"
#include "StatsFunc.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  stats(ui);
}
