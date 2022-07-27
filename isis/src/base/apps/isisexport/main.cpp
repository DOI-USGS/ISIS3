#include "Isis.h"

#include "isisexport.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;

  isisexport(ui, &appLog); 
}
