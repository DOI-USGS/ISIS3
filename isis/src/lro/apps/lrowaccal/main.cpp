#include "Isis.h"

#include "lrowaccal.h"

using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  lrowaccal(ui);
}