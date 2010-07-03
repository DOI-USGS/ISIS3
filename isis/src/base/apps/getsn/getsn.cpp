#include "Isis.h"

#include <iostream>
#include <sstream>
#include <string>

#include "Pvl.h"
#include "Cube.h"
#include "Blob.h"
#include "History.h"
#include "SerialNumber.h"
#include "SessionLog.h"
#include "ObservationNumber.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

void IsisMain() {
  // Set Preferences to always turn off Terminal Output
  PvlGroup &grp = Isis::Preference::Preferences().FindGroup("SessionLog", Isis::Pvl::Traverse);
  grp["TerminalOutput"].SetValue("Off");
       
  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  Cube cube;
  string from = ui.GetFilename("FROM");
  cube.Open(from, "r");

  // Determine if output should be written base on parameters
  bool WriteFile = ui.GetBoolean("FILE");
  bool WriteSN = ui.GetBoolean("SN");
  bool WriteObservation = ui.GetBoolean("OBSERVATION");

  // Extract label from cube file
  Pvl *label = cube.Label();

  PvlGroup sn("Results");

  if(WriteFile) sn += PvlKeyword("Filename",from);
  if(WriteSN) sn += PvlKeyword("SerialNumber",SerialNumber::Compose(*label,ui.GetBoolean("DEFAULT")));
  if(WriteObservation) sn += PvlKeyword("ObservationNumber",ObservationNumber::Compose(*label,ui.GetBoolean("DEFAULT")));

  if(ui.WasEntered("TO")) {
    // Create a serial number and observation number for this cube & put it in a pvlgroup for output
    Pvl pvl;
    pvl.AddGroup( sn );
    if( ui.GetBoolean("APPEND") )
      pvl.Append( ui.GetFilename("TO") );
    else
      pvl.Write( ui.GetFilename("TO") );

    // Construct a label with the results
    if (ui.IsInteractive()) {
      Application::GuiLog(sn);
    }
  }
  else {
    if(WriteFile) cout << from << endl;
    if(WriteSN) cout << SerialNumber::Compose(*label,ui.GetBoolean("DEFAULT")) << endl;
    if(WriteObservation) cout << ObservationNumber::Compose(*label,ui.GetBoolean("DEFAULT")) << endl;
  }
  // Write the results to the log but not the terminal
  SessionLog::TheLog().AddResults(sn);
}
