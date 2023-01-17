#include "Isis.h"

#include "spiceinit.h"

#include "Application.h"
#include "IException.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  spiceinit(ui, &appLog); 
}
