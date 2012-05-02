#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "Table.h"
#include "FileName.h"
#include "ImportPdsTable.h"

using namespace std;
using namespace Isis;

void IsisMain ()
{
  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  FileName inFile = ui.GetFileName("FROM");
  string imageFile("");
  if (ui.WasEntered("IMAGE")) {
    imageFile = ui.GetFileName("IMAGE");
  }


  // Generate the housekeeping filenames
  string hkLabel("");
  string hkData("");
  if (ui.WasEntered("HKFROM") ) {
    hkLabel = ui.GetFileName("HKFROM");
  }
  else {
    hkLabel = inFile.originalPath() + "/" + inFile.baseName() + "_HK.LBL";
    // Determine the housekeeping file
    FileName hkFile(hkLabel);
    if (!hkFile.fileExists()) {
      hkFile = iString::Replace(hkLabel, "_1B_", "_1A_", false);
      if (hkFile.fileExists()) hkLabel = hkFile.expanded();
    }
  }

  if (ui.WasEntered("HKTABLE")) {
    hkData = ui.GetFileName("HKTABLE");
  }

  iString instid;
  iString missid;

  try {
    Pvl lab(inFile.expanded());
    instid = (string) lab.FindKeyword ("CHANNEL_ID");
    missid = (string) lab.FindKeyword ("INSTRUMENT_HOST_ID");
  }
  catch (IException &e) {
    string msg = "Unable to read [INSTRUMENT_ID] or [MISSION_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }

  instid.ConvertWhiteSpace();
  instid.Compress();
  instid.Trim(" ");
  missid.ConvertWhiteSpace();
  missid.Compress();
  missid.Trim(" ");
  if (missid != "DAWN" && instid != "VIS" && instid != "IR") {
    string msg = "Input file [" + inFile.expanded() + "] does not appear to be a " +
                 "DAWN Visual and InfraRed Mapping Spectrometer (VIR) EDR or RDR file.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  std::string target;
  if(ui.WasEntered("TARGET")) {
    target = ui.GetString("TARGET");
  }

//  p.SetPdsFile (inFile.expanded(),imageFile,pdsLabel);
//  string labelFile = ui.GetFileName("FROM");
  p.SetPdsFile (inFile.expanded(),imageFile,pdsLabel);
  p.SetOrganization(Isis::ProcessImport::BIP);
  Cube *outcube = p.SetOutputCube ("TO");
//  p.SaveFileHeader();

  Pvl labelPvl (inFile.expanded());

  p.StartProcess ();

  // Get the directory where the DAWN translation tables are.
  PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Dawn"] + "/translations/";

  // Create a PVL to store the translated labels in
  Pvl outLabel;

  // Translate the BandBin group
  FileName transFile (transDir + "dawnvirBandBin.trn");
  PvlTranslationManager bandBinXlater (labelPvl, transFile.expanded());
  bandBinXlater.Auto(outLabel);

  // Translate the Archive group
  transFile = transDir + "dawnvirArchive.trn";
  PvlTranslationManager archiveXlater (labelPvl, transFile.expanded());
  archiveXlater.Auto(outLabel);

  // Translate the Instrument group
  transFile = transDir + "dawnvirInstrument.trn";
  PvlTranslationManager instrumentXlater (labelPvl, transFile.expanded());
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
    string msg = "Input file [" + inFile.expanded() + "] has an invalid " +
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
