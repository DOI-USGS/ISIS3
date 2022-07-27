#include "Isis.h"

#include "shadow.h"

#include "Application.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  shadow(ui, &appLog); 
}
