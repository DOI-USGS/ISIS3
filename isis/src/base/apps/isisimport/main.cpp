#include "Isis.h"

#include "isisimport.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;

  isisimport(ui, &appLog); 
}
