#include "Isis.h"

#include "Application.h"
#include "ctxcal.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ctxcal(ui);
}