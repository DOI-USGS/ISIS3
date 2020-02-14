#include "Isis.h"

#include "cnetwinnow.h"

#include "Application.h"
#include "UserInterface.h"


using namespace std;
using namespace Isis;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  cnetwinnow(ui);
}
