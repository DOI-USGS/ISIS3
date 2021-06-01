#include "Isis.h"

#include "Application.h"
#include "segment.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  segment(ui);
}
