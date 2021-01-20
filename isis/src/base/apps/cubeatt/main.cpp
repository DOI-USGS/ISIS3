#include "Isis.h"

#include "cubeatt.h"

#include "Application.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  cubeatt(ui);
}