#include "Isis.h"

#include "apollopanstitcher.h"

#include "Application.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  apolloPanStitcher(ui);
}
