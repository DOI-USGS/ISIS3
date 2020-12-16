#include "Isis.h"

#include "topds4.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  PvlGroup results = topds4(ui);
  Application::Log(results);
}
