#include "Isis.h"

#include "enlarge_app.h"

#include "Application.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  enlarge(ui, &appLog); 
}