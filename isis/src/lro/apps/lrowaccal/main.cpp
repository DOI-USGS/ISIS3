#include "Isis.h"

#include "Application.h"
#include "lrowaccal.h"

using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  lrowaccal(ui);
}