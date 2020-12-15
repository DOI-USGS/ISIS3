#include "Isis.h"

#include "Application.h"
#include "bandnorm.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  bandnorm(ui);
}