#include "Isis.h"

#include "isis2ascii.h"
#include "Application.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  isis2ascii(ui);
}
