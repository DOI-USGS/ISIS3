#include "Isis.h"

#include "himos.h"

#include "Application.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  himos(ui);
}
