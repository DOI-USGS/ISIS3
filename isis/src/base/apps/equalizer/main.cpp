#include "Isis.h"
#include "equalizer.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Isis::equalizer(ui);
}