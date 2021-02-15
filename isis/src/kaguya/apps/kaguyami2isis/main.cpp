#include "Isis.h"

#include "kaguyami2isis.h"

#include "Application.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  kaguyami2isis(ui);
}
