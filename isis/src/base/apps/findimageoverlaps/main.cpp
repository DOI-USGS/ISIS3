#include "Isis.h"

#include "findimageoverlaps.h"

#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  findimageoverlaps(ui);
}
