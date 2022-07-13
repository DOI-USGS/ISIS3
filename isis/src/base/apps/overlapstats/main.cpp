#include "Isis.h"

#include "Application.h"
#include "Process.h"
#include "UserInterface.h"

#include "overlapstats.h"

using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  overlapstats(ui, &appLog); 
}
