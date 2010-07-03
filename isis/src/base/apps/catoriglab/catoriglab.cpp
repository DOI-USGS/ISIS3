#include "Isis.h"
#include "Pvl.h"
#include "OriginalLabel.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  // Get user entered file name & mode 
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFilename("FROM");

  // Extract history from file
  OriginalLabel origLab(file);
  Pvl pvl = origLab.ReturnLabels();
  cout << pvl << endl;
}
