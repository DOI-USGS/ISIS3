#include "Isis.h"

#include "grid.h"

#include "Application.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  grid(ui);
}