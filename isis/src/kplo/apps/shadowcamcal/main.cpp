#include "Isis.h"

#include "Application.h"
#include "UserInterface.h"

#include "shadowcamcal.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  shadowcamcal(ui);
}