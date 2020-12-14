#include "Isis.h"
#include "UserInterface.h"
#include "hrsc2isis.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hrsc2isis(ui);
}
