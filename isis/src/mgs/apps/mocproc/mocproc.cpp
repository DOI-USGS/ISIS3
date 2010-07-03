#include "Isis.h"

#include <string>

#include "ProcessImport.h"

#include "Filename.h"
#include "iException.h"
#include "iTime.h"
#include "Pipeline.h"

using namespace std; 
using namespace Isis;

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();

  if (!ui.GetBoolean("INGESTION") && 
      !ui.GetBoolean("CALIBRATION") &&
      !ui.GetBoolean("MAPPING")) {
    string m = "Please pick at least one of [INGESTION, CALIBRATION, MAPPING]";
    throw iException::Message(iException::User,m,_FILEINFO_);
  }
 
  Pipeline p("mocproc");

  p.SetInputFile("FROM", "");
  p.SetOutputFile("TO");

  p.KeepTemporaryFiles(false);

  if(ui.GetBoolean("Ingestion")) {
    p.AddToPipeline("moc2isis");
    p.Application("moc2isis").SetInputParameter("FROM", false);
    p.Application("moc2isis").SetOutputParameter("TO", "lev0");
  
    p.AddToPipeline("spiceinit");
    p.Application("spiceinit").SetInputParameter("FROM", false);
    p.Application("spiceinit").AddParameter("PCK", "PCK");
    p.Application("spiceinit").AddParameter("CK", "CK");
    p.Application("spiceinit").AddParameter("SPK", "SPK");
    p.Application("spiceinit").AddParameter("CKNADIR", "CKNADIR");
    p.Application("spiceinit").AddParameter("SHAPE", "SHAPE");
    p.Application("spiceinit").AddParameter("MODEL", "MODEL");
  }

  if(ui.GetBoolean("Calibration")) {
    p.AddToPipeline("moccal");
    p.Application("moccal").SetInputParameter("FROM", true);
    p.Application("moccal").SetOutputParameter("TO", "lev1");
  
    p.AddToPipeline("mocnoise50");
    p.Application("mocnoise50").SetInputParameter("FROM", true);
    p.Application("mocnoise50").SetOutputParameter("TO", "noise");
  
    p.AddToPipeline("mocevenodd");
    p.Application("mocevenodd").SetInputParameter("FROM", true);
    p.Application("mocevenodd").SetOutputParameter("TO", "evenodd");

    Pvl inputPvl(Filename(ui.GetFilename("FROM")).Expanded());

    int summingMode = 0;
    bool isNarrowAngle = false;

    if(inputPvl.HasKeyword("CROSSTRACK_SUMMING")) {
      summingMode = inputPvl["CROSSTRACK_SUMMING"];
      isNarrowAngle = ((string)inputPvl["INSTRUMENT_ID"] == "MOC-NA");
    }
    else {
      PvlGroup &inst = inputPvl.FindGroup("Instrument", Pvl::Traverse);
      summingMode = inst["CrosstrackSumming"];
      isNarrowAngle = ((string)inst["InstrumentId"] == "MOC-NA");
    }

    if(summingMode != 1) {
      p.Application("mocnoise50").Disable();
      p.Application("mocevenodd").Disable();
    }

    if(!isNarrowAngle) {
      p.Application("mocnoise50").Disable();
    }
  }

  if(ui.GetBoolean("Mapping")) {
    p.AddToPipeline("cam2map");
    p.Application("cam2map").SetInputParameter("FROM", true);
    p.Application("cam2map").SetOutputParameter("TO", "lev2");
    p.Application("cam2map").AddParameter("MAP", "MAP");
    p.Application("cam2map").AddParameter("PIXRES", "RESOLUTION");
  
    if (ui.WasEntered("PIXRES")) {
      p.Application("cam2map").AddConstParameter("PIXRES", "MPP");
    }
    else if(ui.WasEntered("MAP")) {
      Pvl mapPvl(Filename(ui.GetFilename("MAP")).Expanded());
      if (mapPvl.FindGroup("Mapping", Pvl::Traverse).HasKeyword("PixelResolution")) {
        p.Application("cam2map").AddConstParameter("PIXRES", "MAP");
      }
    }
  }

  p.Run();
}
