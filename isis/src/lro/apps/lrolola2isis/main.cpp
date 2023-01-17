#include "Isis.h"

#include "Application.h"
#include "UserInterface.h"
#include "lrolola2isis.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  lrolola2isis(ui); 
}
