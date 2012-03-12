#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "Table.h"
#include "Filename.h"
#include "ImportPdsTable.h"

using namespace std;
using namespace Isis;

void IsisMain ()
{
  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  Filename inFile = ui.GetFilename("FROM");
  string imageFile("");
  if (ui.WasEntered("IMAGE")) {
    imageFile = ui.GetFilename("IMAGE");
  }


  // Generate the housekeeping filenames
  string hkLabel("");
  string hkData("");
  if (ui.WasEntered("HKFROM") ) {
    hkLabel = ui.GetFilename("HKFROM");
  }
  else {
    hkLabel = inFile.OriginalPath() + "/" + inFile.Basename() + "_HK.LBL";
    // Determine the housekeeping file
    Filename hkFile(hkLabel);
    if (!hkFile.Exists()) {
      hkFile = iString::Replace(hkLabel, "_1B_", "_1A_", false);
      if (hkFile.Exists()) hkLabel = hkFile.Expanded();
    }
  }

  if (ui.WasEntered("HKTABLE")) {
    hkData = ui.GetFilename("HKTABLE");
  }

  iString instid;
  iString missid;

  try {
    Pvl lab(inFile.Expanded());
    instid = (string) lab.FindKeyword ("CHANNEL_ID");
    missid = (string) lab.FindKeyword ("INSTRUMENT_HOST_ID");
  }
  catch (IException &e) {
    string msg = "Unable to read [INSTRUMENT_ID] or [MISSION_ID] from input file [" +
                 inFile.Expanded() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }

  instid.ConvertWhiteSpace();
  instid.Compress();
  instid.Trim(" ");
  missid.ConvertWhiteSpace();
  missid.Compress();
  missid.Trim(" ");
  if (missid != "DAWN" && instid != "VIS" && instid != "IR") {
    string msg = "Input file [" + inFile.Expanded() + "] does not appear to be a " +
                 "DAWN Visual and InfraRed Mapping Spectrometer (VIR) EDR or RDR file.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  std::string target;
  if(ui.WasEntered("TARGET")) {
    target = ui.GetString("TARGET");
  }

//  p.SetPdsFile (inFile.Expanded(),imageFile,pdsLabel);
//  string labelFile = ui.GetFilename("FROM");
  p.SetPdsFile (inFile.Expanded(),imageFile,pdsLabel);
  p.SetOrganization(Isis::ProcessImport::BIP);
  Cube *outcube = p.SetOutputCube ("TO");
//  p.SaveFileHeader();

  Pvl labelPvl (inFile.Expanded());

  p.StartProcess ();

  // Get the directory where the DAWN translation tables are.
  PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Dawn"] + "/translations/";

  // Create a PVL to store the translated labels in
  Pvl outLabel;

  // Translate the BandBin group
  Filename transFile (transDir + "dawnvirBandBin.trn");
  PvlTranslationManager bandBinXlater (labelPvl, transFile.Expanded());
  bandBinXlater.Auto(outLabel);

  // Translate the Archive group
  transFile = transDir + "dawnvirArchive.trn";
  PvlTranslationManager archiveXlater (labelPvl, transFile.Expanded());
  archiveXlater.Auto(outLabel);

  // Translate the Instrument group
  transFile = transDir + "dawnvirInstrument.trn";
  PvlTranslationManager instrumentXlater (labelPvl, transFile.Expanded());
  instrumentXlater.Auto(outLabel);

  //  Update target if user specifies it
  if (!target.empty()) {
    PvlGroup &igrp = outLabel.FindGroup("Instrument",Pvl::Traverse);
    igrp["TargetName"] = iString(target);
  }

  // Write the BandBin, Archive, and Instrument groups
  // to the output cube label
  outcube->putGroup(outLabel.FindGroup("BandBin",Pvl::Traverse));
  outcube->putGroup(outLabel.FindGroup("Archive",Pvl::Traverse));
  outcube->putGroup(outLabel.FindGroup("Instrument",Pvl::Traverse));

  PvlGroup kerns("Kernels");
  if (instid == "VIS") {
    kerns += PvlKeyword("NaifFrameCode",-203211);
  } else if (instid == "IR") {
    kerns += PvlKeyword("NaifFrameCode",-203213);
  } else {
    string msg = "Input file [" + inFile.Expanded() + "] has an invalid " +
                 "InstrumentId.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  outcube->putGroup(kerns);

  // Now handle generation of housekeeping data
 try {
   ImportPdsTable hktable(hkLabel, hkData);
   hktable.setType("ScetTimeClock", "CHARACTER");
   hktable.setType("ShutterStatus", "CHARACTER");
   hktable.setType("MirrorSin", "DOUBLE");
   hktable.setType("MirrorCos", "DOUBLE");
   Table hktab = hktable.exportAsTable("ScetTimeClock,ShutterStatus,MirrorSin,MirrorCos",
                                        "VIRHouseKeeping");
   hktab.Label().AddKeyword(PvlKeyword("SourceFile", hkLabel));
   outcube->write(hktab);
 }
 catch (IException &ie) {
   string mess = "Cannot read/open housekeeping data";
   throw IException(ie, IException::User, mess, _FILEINFO_);
 }

  p.EndProcess ();
}
