#include "Isis.h"

#include "Application.h"
#include "apollofindrx.h" 

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  apollofindrx(ui);
}
