#include "Isis.h"
#include "Pipeline.h"

using namespace Isis;

void ProcessVis(bool isRdr);
void ProcessIr();

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();

  if (!ui.GetBoolean("INGESTION") && !ui.GetBoolean("MAPPING")) {
    string msg = "You must pick one of [INGESTION,MAPPING]";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
 
  if(ui.GetBoolean("INGESTION")) {
    Pvl labels(ui.GetFilename("FROM"));

    if((iString)labels["DETECTOR_ID"][0] == "VIS") {
      if(((string)labels["DATA_SET_ID"]).find("RDR") != string::npos) {
        ProcessVis(true);
      }
      else {
        ProcessVis(false);
      }
    }
    else {
      ProcessIr();
    }
  }
  else {
    ProcessIr();
  }
}

void ProcessVis(bool isRdr) {
  UserInterface &ui = Application::GetUserInterface();
  Pipeline p("thmproc");

  p.SetInputFile("FROM", "BANDS");
  p.SetOutputFile("TO");
  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  p.AddToPipeline("thm2isis");
  p.Application("thm2isis").SetInputParameter("FROM", false);
  p.Application("thm2isis").SetOutputParameter("TO", "raw");
  p.Application("thm2isis").AddBranch("even", PipelineApplication::ConstantStrings);
  p.Application("thm2isis").AddBranch("odd", PipelineApplication::ConstantStrings);

  p.AddToPipeline("spiceinit");
  p.Application("spiceinit").SetInputParameter("FROM", false);
  p.Application("spiceinit").AddParameter("PCK", "PCK");
  p.Application("spiceinit").AddParameter("CK", "CK");
  p.Application("spiceinit").AddParameter("SPK", "SPK");
  p.Application("spiceinit").AddParameter("SHAPE", "SHAPE");
  p.Application("spiceinit").AddParameter("MODEL", "MODEL");
  p.Application("spiceinit").AddParameter("CKNADIR", "CKNADIR");

  p.AddToPipeline("thmvisflat");
  p.Application("thmvisflat").SetInputParameter("FROM", true);
  p.Application("thmvisflat").SetOutputParameter("TO", "flat");

  p.AddToPipeline("thmvistrim");
  p.Application("thmvistrim").SetInputParameter("FROM", true);
  p.Application("thmvistrim").SetOutputParameter("TO", "cal");

  if(!ui.GetBoolean("VISCLEANUP")) {
    p.Application("thmvisflat").Disable();
    p.Application("thmvistrim").Disable();
  }

  if(isRdr) {
    p.Application("thmvisflat").Disable();
  }

  p.AddToPipeline("cam2map");
  p.Application("cam2map").SetInputParameter("FROM", true);
  p.Application("cam2map").SetOutputParameter("TO", "lev2");

  p.Application("cam2map").AddParameter("even", "MAP", "MAP");
  p.Application("cam2map").AddParameter("even", "PIXRES", "RESOLUTION");

  if(ui.WasEntered("PIXRES")) {
    p.Application("cam2map").AddConstParameter("even", "PIXRES", "MPP");
  }

  p.Application("cam2map").AddParameter("odd", "MAP", PipelineApplication::LastOutput);
  p.Application("cam2map").AddConstParameter("odd", "PIXRES", "MAP");
  p.Application("cam2map").AddConstParameter("odd", "DEFAULTRANGE", "MAP");

  p.AddToPipeline("automos");
  p.Application("automos").SetInputParameter("FROMLIST", PipelineApplication::LastAppOutputList, false);
  p.Application("automos").SetOutputParameter("MOSAIC", "mos");

  if(ui.GetBoolean("INGESTION")) {
    p.SetFirstApplication("thm2isis");
  }
  else{
    p.SetFirstApplication("spiceinit");
  }

  if(ui.GetBoolean("MAPPING")) {
    p.SetLastApplication("automos");
  }
  else {
    p.SetLastApplication("thmvistrim");
  }

  p.Run();
}

void ProcessIr() {
  UserInterface &ui = Application::GetUserInterface();
  Pipeline p;

  p.SetInputFile("FROM", "BANDS");
  p.SetOutputFile("TO");

  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  p.AddToPipeline("thm2isis");
  p.Application("thm2isis").SetInputParameter("FROM", false);
  p.Application("thm2isis").SetOutputParameter("TO", "raw");

  p.AddToPipeline("spiceinit");
  p.Application("spiceinit").SetInputParameter("FROM", false);
  p.Application("spiceinit").AddParameter("PCK", "PCK");
  p.Application("spiceinit").AddParameter("CK", "CK");
  p.Application("spiceinit").AddParameter("SPK", "SPK");
  p.Application("spiceinit").AddParameter("SHAPE", "SHAPE");
  p.Application("spiceinit").AddParameter("MODEL", "MODEL");
  p.Application("spiceinit").AddParameter("CKNADIR", "CKNADIR");

  p.AddToPipeline("cam2map");
  p.Application("cam2map").SetInputParameter("FROM", true);
  p.Application("cam2map").SetOutputParameter("TO", "lev2");
  p.Application("cam2map").AddParameter("MAP", "MAP");
  p.Application("cam2map").AddParameter("PIXRES", "RESOLUTION");

  if(ui.WasEntered("PIXRES")) {
    p.Application("cam2map").AddConstParameter("PIXRES", "MPP");
  }

  if(ui.GetBoolean("INGESTION")) {
    p.SetFirstApplication("thm2isis");
  }
  else{
    p.SetFirstApplication("spiceinit");
  }

  if(ui.GetBoolean("MAPPING")) {
    p.SetLastApplication("cam2map");
  }
  else {
    p.SetLastApplication("spiceinit");
  }

  p.Run();
}
