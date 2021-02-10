#include "Isis.h"

#include "Application.h"
#include "lorri2isis.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  lorri2isis(ui);
}
