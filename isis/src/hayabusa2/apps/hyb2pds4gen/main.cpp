#include "Isis.h"

#include "hyb2pds4gen.h"

#include "Application.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hyb2pds4gen(ui);
}