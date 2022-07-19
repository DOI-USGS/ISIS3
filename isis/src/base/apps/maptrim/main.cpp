#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "maptrim.h" 

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  maptrim(ui, &appLog); 
}
