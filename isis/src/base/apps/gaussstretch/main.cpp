#include "Isis.h"

#include "gaussstretch.h"
#include "Application.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  gaussstretch(ui);
}
