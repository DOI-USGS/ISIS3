//#define GUIHELPERS

#include "Isis.h"
#include "map2map.h"
#include "UserInterface.h"
#include "Pvl.h"
#include "Application.h"

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
  Pvl app_log;
  map2map(ui, &app_log);
  //Pvl results = map2map(ui);
  for (int resultIndex = 0; resultIndex < app_log.groups(); resultIndex++) {
    Application::Log(app_log.group(resultIndex));
  }
}
