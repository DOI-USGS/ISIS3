#include "Isis.h"

#include "equalizer.h"

#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  equalizer(ui);
}
