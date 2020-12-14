#include "Isis.h"

#include "Application.h"
#include "hicolormos.h" 

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hicolormos(ui);
}