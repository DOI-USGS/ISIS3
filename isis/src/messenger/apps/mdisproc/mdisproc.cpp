#include "Isis.h"

#include <string>

#include "UserInterface.h"
#include "Pipeline.h"
#include "iException.h"

using namespace std;
using namespace Isis;

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();
  if (!ui.GetBoolean("INGESTION") && !ui.GetBoolean("CALIBRATION") && !ui.GetBoolean("CDR")) {
    string msg = "You must pick one of [INGESTION,CALIBRATION,CDR]";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  Pipeline p("mdisproc");

  p.SetInputFile("FROM","BANDS");
  p.SetOutputFile("TO");

  p.KeepTemporaryFiles( !ui.GetBoolean("REMOVE") );

  //---------------------------------------------------------------------------
  // Set up the ingestion run if requested
  if (ui.GetBoolean("INGESTION")) {
    string app = "mdis2isis";
    p.AddToPipeline(app);
    p.Application(app).SetInputParameter("FROM",false);
    p.Application(app).SetOutputParameter("TO","raw");
  }

  //---------------------------------------------------------------------------
  // Set up the calibration run if requested
  if( ui.GetBoolean("CALIBRATION") ) {
    string app = "mdiscal";
    p.AddToPipeline(app);
    p.Application(app).SetInputParameter("FROM",true);
    p.Application(app).SetOutputParameter("TO","lev1");
    p.Application(app).AddParameter("DARKCURRENT","DARKCURRENT");
    if ( ui.GetInteger("TRIM") == 0  ||  ui.GetInteger("TRIM") == 3 ) {
      p.Application(app).AddConstParameter("KEEPDARK","true");
    }
    p.Application(app).AddParameter("IOF","IOF");

    // trim if needed
    if ( ui.GetInteger("TRIM") != 0  &&  ui.GetInteger("TRIM") != 3 ) {
      app = "trim";
      p.AddToPipeline(app);
      p.Application(app).SetInputParameter("FROM",true);
      p.Application(app).SetOutputParameter("TO","trim");
      p.Application(app).AddParameter("TRIM","LEFT");
    }
  }

  //---------------------------------------------------------------------------
  // Set up the calibrated run when asked
  if( ui.GetBoolean("CDR") ) {
    string app = "mdis2pds";
    p.AddToPipeline(app);
    p.Application(app).SetInputParameter("FROM",true);
    p.Application(app).SetOutputParameter("TO","cdr","IMG");
    p.Application(app).AddParameter("BITS","BITS");
  }

  p.Run();

}
