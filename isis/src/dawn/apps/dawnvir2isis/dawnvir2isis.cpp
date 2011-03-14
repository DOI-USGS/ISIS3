#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "Table.h"
#include "Filename.h"

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

  iString instid;
  iString missid;

  try {
    Pvl lab(inFile.Expanded());
    instid = (string) lab.FindKeyword ("CHANNEL_ID");
    missid = (string) lab.FindKeyword ("INSTRUMENT_HOST_ID");
  }
  catch (iException &e) {
    string msg = "Unable to read [INSTRUMENT_ID] or [MISSION_ID] from input file [" +
                 inFile.Expanded() + "]";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
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
    throw iException::Message(iException::Io,msg, _FILEINFO_);
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

  // Save off line suffix data
//  TableField lineClock("
//  p.SetDataSuffixBytes(4);
//  p.SaveDataSuffix();

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
  outcube->PutGroup(outLabel.FindGroup("BandBin",Pvl::Traverse));
  outcube->PutGroup(outLabel.FindGroup("Archive",Pvl::Traverse));
  outcube->PutGroup(outLabel.FindGroup("Instrument",Pvl::Traverse));
  
  // Make sure the HorizontalPixelScale and VerticalPixelScale are the same
  PvlGroup &instGrp(outLabel.FindGroup("Instrument", Pvl::Traverse));
  iString sheight = (string) instGrp["VerticalPixelScale"];
  iString swidth = (string) instGrp["HorizontalPixelScale"];
  if ((sheight == "N/A" && swidth != "N/A") || (sheight != "N/A" &&
      swidth == "N/A")) {
    string msg = "Input file [" + inFile.Expanded() + "] does not have valid " +
                 "HorizontalPixelScale and VerticalPixelScale values. These values " +
                 "must be equivalent or the image is considered to be invalid.";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }
  if (sheight != "N/A" && swidth != "N/A") {
    double pheight = sheight.ToDouble();
    double pwidth = swidth.ToDouble();
    if (pheight != pwidth) {
      string msg = "Input file [" + inFile.Expanded() + "] does not have valid " +
                   "HorizontalPixelScale and VerticalPixelScale values. These values " +
                   "must be equivalent or the image is considered to be invalid.";
      throw iException::Message(iException::Io,msg, _FILEINFO_);
    }
  }


  PvlGroup kerns("Kernels");
  if (instid == "VIS") {
    kerns += PvlKeyword("NaifFrameCode",-203211);
  } else if (instid == "IR") {
    kerns += PvlKeyword("NaifFrameCode",-203213);
  } else {
    string msg = "Input file [" + inFile.Expanded() + "] has an invalid " +
                 "InstrumentId.";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }
  outcube->PutGroup(kerns);

  p.EndProcess ();
}
