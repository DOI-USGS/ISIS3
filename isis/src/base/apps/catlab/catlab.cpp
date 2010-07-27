#include "Isis.h"

#include <iostream>

#include "Filename.h"
#include "Pvl.h"
#include "UserInterface.h"

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
    if (ui.GetBoolean("APPEND")) {
      label.Append(Filename(ui.GetFilename("TO")).Expanded());
    }
    else {
      label.Write(Filename(ui.GetFilename("TO")).Expanded());
    }
  }

  // Print label to the gui log if it is interactive
  else if(ui.IsInteractive()) {
    Application::GuiLog(label);
  }

  // Print label to the screen if it is not
  else {
    cout << label << endl;
  }

}


