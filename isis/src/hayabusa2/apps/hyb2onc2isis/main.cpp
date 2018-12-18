#include "Isis.h"

#include "hyb2onc2isis.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();
  hyb2onc2isis(ui);

}

