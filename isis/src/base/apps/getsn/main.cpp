#include "Isis.h"

#include <iostream>
#include <sstream>
#include <QString>

#include "Blob.h"
#include "Cube.h"
#include "History.h"
#include "ObservationNumber.h"
#include "Preference.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "SessionLog.h"
#include "TextFile.h"

using namespace Isis;
using namespace std;

void IsisMain() {
  // Set Preferences to always turn off Terminal Output
  PvlGroup &grp = Isis::Preference::Preferences().findGroup("SessionLog", Isis::Pvl::Traverse);
  grp["TerminalOutput"].setValue("Off");

  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  Cube cube;
  QString from = ui.GetFileName("FROM");
  cube.open(from, "r");

  // Determine if output should be written base on parameters
  bool WriteFile = ui.GetBoolean("FILE");
  bool WriteSN = ui.GetBoolean("SN");
  bool WriteObservation = ui.GetBoolean("OBSERVATION");

  QString format = ui.GetString("FORMAT");
  bool pvl;
  if (format == "PVL") {
    pvl = true;
  }
  else {
    pvl = false;
  }

  // Extract label from cube file
  Pvl *label = cube.label();

  PvlGroup sn("Results");

  if (WriteFile) sn += PvlKeyword("Filename", from);
  if (WriteSN) sn += PvlKeyword( "SerialNumber", SerialNumber::Compose( *label, ui.GetBoolean("DEFAULT") ) );
  if (WriteObservation) sn += PvlKeyword( "ObservationNumber", ObservationNumber::Compose( *label, ui.GetBoolean("DEFAULT") ) );

  if ( ui.WasEntered("TO") ) {
    // PVL option
    if (pvl) {
      // Create a serial number and observation number for this cube & put it in a pvlgroup for output
      Pvl pvl;
      pvl.addGroup(sn);
      if ( ui.GetBoolean("APPEND") )
        pvl.append( ui.GetFileName("TO") );
      else
        pvl.write( ui.GetFileName("TO") );
    }
    // FLAT option
    else {
      bool append = ui.GetBoolean("APPEND");
      // Open in append or overwrite
      TextFile txt(ui.GetFileName("TO"), append ? "append" : "overwrite");

      // Build QString
      QString line = "";
      for (int i = 0; i < sn.keywords(); i++) {
        if (i != 0)
          line += ",";
        line += sn[i][0];
      }
      txt.PutLine(line);
    }

    // Construct a label with the results
    if ( ui.IsInteractive() ) {
      Application::GuiLog(sn);
    }
  }
  else {
    for (int i = 0; i < sn.keywords(); i++) {
      cout << sn[i][0] << endl;
    }
  }
  // Write the results to the log but not the terminal
  SessionLog::TheLog().AddResults(sn);
}
