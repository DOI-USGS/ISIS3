#include "Isis.h"

#include "Application.h"
#include "hicubenorm.h" 

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hicubenorm(ui);
}
