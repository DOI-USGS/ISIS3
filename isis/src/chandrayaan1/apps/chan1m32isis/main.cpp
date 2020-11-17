#include "Isis.h"
#include "chan1m32isis.h"
#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl results = chan1m32isis(ui);
  for (int resultIndex = 0; resultIndex < results.groups(); resultIndex++) {
    Application::Log(results.group(resultIndex));
  }
}
