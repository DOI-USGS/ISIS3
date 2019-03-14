#include "Isis.h"

#include "UserInterface.h"
#include "stats.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  stats(ui);
}
