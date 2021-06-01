/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include <string>

#include "UserInterface.h"
#include "Pipeline.h"
#include "IException.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  if(!ui.GetBoolean("INGESTION") && !ui.GetBoolean("CALIBRATION") && !ui.GetBoolean("CDR")) {
    string msg = "You must pick one of [INGESTION,CALIBRATION,CDR]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  Pipeline p("mdisproc");

  p.SetInputFile("FROM", "BANDS");
  p.SetOutputFile("TO");

  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  //---------------------------------------------------------------------------
  // Set up the ingestion run if requested
  if(ui.GetBoolean("INGESTION")) {
    QString app = "mdis2isis";
    p.AddToPipeline(app);
    p.Application(app).SetInputParameter("FROM", false);
    p.Application(app).SetOutputParameter("TO", "raw");
  }

  //---------------------------------------------------------------------------
  // Set up the calibration run if requested
  if(ui.GetBoolean("CALIBRATION")) {
    QString app = "mdiscal";
    p.AddToPipeline(app);
    p.Application(app).SetInputParameter("FROM", true);
    p.Application(app).SetOutputParameter("TO", "lev1");
    p.Application(app).AddParameter("DARKCURRENT", "DARKCURRENT");
    if(ui.GetInteger("TRIM") == 0  ||  ui.GetInteger("TRIM") == 3) {
      p.Application(app).AddConstParameter("KEEPDARK", "true");
    }
    p.Application(app).AddParameter("IOF", "IOF");

    // trim if needed
    if(ui.GetInteger("TRIM") != 0  &&  ui.GetInteger("TRIM") != 3) {
      app = "trim";
      p.AddToPipeline(app);
      p.Application(app).SetInputParameter("FROM", true);
      p.Application(app).SetOutputParameter("TO", "trim");
      p.Application(app).AddParameter("TRIM", "LEFT");
    }
  }

  //---------------------------------------------------------------------------
  // Set up the calibrated run when asked
  if(ui.GetBoolean("CDR")) {
    QString app = "mdis2pds";
    p.AddToPipeline(app);
    p.Application(app).SetInputParameter("FROM", true);
    p.Application(app).SetOutputParameter("TO", "cdr", "IMG");
    p.Application(app).AddParameter("BITS", "BITS");
  }

  p.Run();

}
