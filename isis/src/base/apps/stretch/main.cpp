#include "Isis.h"

#include "Application.h"
#include "UserInterface.h"

#include "stretch.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl results;
  stretch(ui, &results);
  for (int resultIndex = 0; resultIndex < results.groups(); resultIndex++) {
    Application::Log(results.group(resultIndex));
  }
}
