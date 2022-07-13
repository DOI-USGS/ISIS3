#include "Isis.h"

#include "Application.h"
#include "Process.h"
#include "UserInterface.h"

#include "pds2isis.h"

using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  pds2isis(ui, &appLog); 
}
