#include "Isis.h"

#include "hicubeit.h"

#include "Application.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hicubeit(ui);
}
