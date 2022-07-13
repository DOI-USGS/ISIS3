#include "Isis.h"

#include "campt.h"

#include "Application.h"
#include "PvlLog.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  campt(ui, &appLog);
}