#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "isis2pds.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  isis2pds(ui, &appLog); 
}
