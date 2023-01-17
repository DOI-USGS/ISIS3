#include "Isis.h"

#include "Application.h"
#include "Process.h"
#include "UserInterface.h"

#include "mapmos.h"

using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  mapmos(ui, &appLog); 
}
