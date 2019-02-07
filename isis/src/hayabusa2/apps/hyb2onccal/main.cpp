#include "Isis.h"

#include "UserInterface.h"
#include "hyb2onccal.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hyb2onccal(ui);
}
