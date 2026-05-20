#include "Isis.h"
#include "handmos.h"
#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  handmos(ui);
}
