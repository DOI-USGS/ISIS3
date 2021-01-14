#include "Isis.h"
#include "lo2isis.h"
#include "Application.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  lo2isis(ui);
}
