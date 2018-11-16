#include "Isis.h"

#include "crop_func.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  PvlGroup results = crop(ui);
  Application::Log(results);
}
