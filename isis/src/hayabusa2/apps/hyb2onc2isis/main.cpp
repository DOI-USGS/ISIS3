#include "Isis.h"
#include "UserInterface.h"

#include "hyb2onc2isis.h"


using namespace std;
using namespace Isis;

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();
  hyb2onc2isis(ui);

}

