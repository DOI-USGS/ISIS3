#include "Isis.h"

#include "Application.h"
#include "Process.h"
#include "UserInterface.h"

#include "camrange.h"

using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  camrange(ui, &appLog);
}
