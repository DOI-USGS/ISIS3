#include "Isis.h"

#include "Application.h"
#include "UserInterface.h"
#include "stretch_app.h"
 
using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl results;
  stretch(ui, &results);
}
