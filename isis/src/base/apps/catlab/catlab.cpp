#include "Isis.h"

#include <iostream>

#include "Pvl.h"

using namespace Isis;
using namespace std;


void IsisMain() {

  // Get filename provided by the user
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFilename("FROM");

  // Extract label from file
  Pvl label(file);

  // Output to file if entered
  if(ui.WasEntered("TO")) {
    label.Write( ui.GetFilename("TO") );
  }

  // Print label to the gui log if it is interactive
  else if (ui.IsInteractive()) {
    Application::GuiLog(label);
  }

  // Print label to the screen if it is not
  else {
    cout << label << endl;
  }

}
       

