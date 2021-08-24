#include "Isis.h"

#include "Application.h"
#include "tgocassis2isis.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  tgocassis2isis(ui);
}