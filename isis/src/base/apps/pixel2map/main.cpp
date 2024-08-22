#include "Isis.h"

#include "pixel2map.h"

#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  pixel2map(ui);
}



  

