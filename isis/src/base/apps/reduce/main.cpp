#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "reduce_app.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  reduce(ui, &appLog); 
}
