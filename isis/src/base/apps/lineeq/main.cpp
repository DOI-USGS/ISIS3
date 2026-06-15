#include "Isis.h"

#include "lineeq.h"

#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  PvlGroup results = lineeq(ui);
  Application::Log(results);
}
