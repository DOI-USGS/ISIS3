#include "Isis.h"

#include "ckwriter.h"

#include "Application.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  ckwriter(ui, &appLog);
}
