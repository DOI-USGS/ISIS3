#include "Isis.h"
#include "dsk2isis.h"
#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  dsk2isis(ui);
}

