#include "Isis.h"

#include "lronac2pds.h"
#include "Application.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  lronac2pds(ui);
}
