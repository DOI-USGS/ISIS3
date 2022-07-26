#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "demprep.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  demprep(ui, &appLog);
 }
