#include "Isis.h"

#include "Application.h"
#include "apollocal.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  apollocal(ui);
}