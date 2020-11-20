#include "Isis.h"

#include "Application.h"
#include "hidtmgen.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hidtmgen(ui);
}
