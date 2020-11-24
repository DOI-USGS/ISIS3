#include "Isis.h"

#include "fits2isis.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  fits2isis(ui);
}
