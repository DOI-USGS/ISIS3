#include "Isis.h"

#include "Application.h"
#include "Pvl.h"

#include "gllssical.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  gllssical(ui, &appLog); 
}