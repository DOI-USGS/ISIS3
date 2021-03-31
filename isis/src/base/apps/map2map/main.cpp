#define GUIHELPERS

#include "Isis.h"
#include "map2map.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;
/*
void PrintMap();
void LoadMapRange();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  helper ["LoadMapRange"] = (void *) LoadMapRange;
  return helper;
}*/


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl results = map2map(ui);
  for (int resultIndex = 0; resultIndex < results.groups(); resultIndex++) {
    Application::Log(results.group(resultIndex));
  }
}
