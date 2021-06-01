#include "Isis.h"

#include "Application.h"
#include "moccal.h"
 
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  moccal(ui);
}