#include "Isis.h"
#include "makeflat.h"
#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  makeflat(ui);
}
