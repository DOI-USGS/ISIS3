#include "Isis.h"
#include "chan1m32isis.h"
#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  chan1m32isis(ui);
}
