#include "Isis.h"

#include "UserInterface.h"
#include "caminfo.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  CamInfo(ui);
}
