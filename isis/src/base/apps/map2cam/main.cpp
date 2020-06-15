#include "Isis.h"
#include "Application.h"
#include "map2cam.h"

using namespace std;
using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  map2cam_f(ui);
}
