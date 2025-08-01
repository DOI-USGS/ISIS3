#include "Isis.h"

#include "crop.h"

using namespace std;
using namespace Isis;

// logic is in crop.cpp
void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  PvlGroup results = crop(ui);
  Application::Log(results);
}
