#include "Isis.h"

#include "UserInterface.h"
#include "framestitch.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  framestitch(ui);
}
