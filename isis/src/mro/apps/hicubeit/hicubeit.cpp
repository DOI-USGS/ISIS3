#include "Isis.h"
#include "TextFile.h"
#include "Pvl.h"
#include "Cube.h"
#include "OriginalLabel.h"
#include <string>

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  string redFile = ui.GetFilename("RED");
  string irFile  = ui.GetFilename("IR");
  string bgFile  = ui.GetFilename("BG");

  Filename tempFile;
  tempFile.Temporary("hicubeit.temp","lis");
  TextFile tf;
  tf.Open(tempFile.Expanded(),"output");
  tf.PutLine(irFile+"\n");
  tf.PutLine(redFile+"\n");
  tf.PutLine(bgFile+"\n");
  tf.Close();

  string parameters = string(" LIST = ")    + tempFile.Expanded() +
                      string(" TO = ")      + ui.GetFilename("TO") +
                      string(" PROPLAB = ") + redFile;
  iApp->Application::Exec("cubeit",parameters);
  remove(tempFile.Expanded().c_str());

  // Get the instrument group from each file
  Pvl redLab(redFile);
  Pvl irLab(irFile);
  Pvl bgLab(bgFile);

  PvlGroup redInst = redLab.FindGroup("Instrument",Pvl::Traverse);
  PvlGroup irInst  = irLab.FindGroup("Instrument",Pvl::Traverse);
  PvlGroup bgInst  = bgLab.FindGroup("Instrument",Pvl::Traverse);

  // Error check to make sure the proper ccds are stacked
  if ((int)redInst["CpmmNumber"] == 5) {
    if (((int)irInst["CpmmNumber"] != 6) || ((int)bgInst["CpmmNumber"] != 4)) {
      string msg = "You can only stack color images with RED4, IR10, and BG12 ";
      msg += "or RED5, IR11, and BG13";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }
  else if ((int)redInst["CpmmNumber"] == 8) {
    if (((int)irInst["CpmmNumber"] != 7) || ((int)bgInst["CpmmNumber"] != 9)) {
      string msg = "You can only stack color images with RED4, IR10, and BG12 ";
      msg += "or RED5, IR11, and BG13";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }
  else {
    string msg = "You can only stack color images with RED4, IR10, and BG12 ";
    msg += "or RED5, IR11, and BG13";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Concatenate all the source products into one keyword
  PvlKeyword sourceProductId("SourceProductId");
  sourceProductId += (string)bgInst["StitchedProductIds"][0];
  if (bgInst["StitchedProductIds"].Size() > 1) {
    sourceProductId += (string)bgInst["StitchedProductIds"][1];
  }
  sourceProductId += (string)redInst["StitchedProductIds"][0];
  if (redInst["StitchedProductIds"].Size() > 1) {
    sourceProductId += (string)redInst["StitchedProductIds"][1];
  }
  sourceProductId += (string)irInst["StitchedProductIds"][0];
  if (irInst["StitchedProductIds"].Size() > 1) {
    sourceProductId += (string)irInst["StitchedProductIds"][1];
  }

  // Get min start and max stop time
  PvlKeyword startTime = redInst["StartTime"];
  PvlKeyword stopTime  = redInst["StopTime"];
  PvlKeyword startClk  = redInst["SpacecraftClockStartCount"];
  PvlKeyword stopClk   = redInst["SpacecraftClockStopCount"];

  if ((string) irInst["StartTime"] < (string)startTime) {
    startTime = irInst["StartTime"];
  }
  if ((string) bgInst["StartTime"] < (string)startTime) {
    startTime = bgInst["StartTime"];
  }

  if ((string) irInst["StopTime"] > (string)stopTime) {
    stopTime = irInst["StopTime"];
  }
  if ((string) bgInst["StopTime"] > (string)stopTime) {
    stopTime = bgInst["StopTime"];
  }

  if ((string) irInst["SpacecraftClockStartCount"] < (string)startClk) {
    startClk = irInst["SpacecraftClockStartCount"];
  }
  if ((string) bgInst["SpacecraftClockStartCount"] < (string)startClk) {
    startClk = bgInst["SpacecraftClockStartCount"];
  }

  if ((string) irInst["SpacecraftClockStopCount"] > (string)stopClk) {
    stopClk = irInst["SpacecraftClockStopCount"];
  }
  if ((string) bgInst["SpacecraftClockStopCount"] > (string)stopClk) {
    stopClk = bgInst["SpacecraftClockStopCount"];
  }

  // Concatenate all TDIs into one keyword
  OriginalLabel redOrgLab;
  redOrgLab.Blob::Read(redFile);
  OriginalLabel irOrgLab;
  irOrgLab.Blob::Read(irFile);
  OriginalLabel bgOrgLab;
  bgOrgLab.Blob::Read(bgFile);

  PvlGroup redGrp = redOrgLab.ReturnLabels().FindGroup("INSTRUMENT_SETTING_PARAMETERS",Pvl::Traverse);
  PvlGroup irGrp = irOrgLab.ReturnLabels().FindGroup("INSTRUMENT_SETTING_PARAMETERS",Pvl::Traverse);
  PvlGroup bgGrp = bgOrgLab.ReturnLabels().FindGroup("INSTRUMENT_SETTING_PARAMETERS",Pvl::Traverse);

  PvlKeyword cpmmTdiFlag("cpmmTdiFlag");
  for (int i=0; i<14; i++) {
    cpmmTdiFlag += (string) "";
  }
  cpmmTdiFlag[(int)redInst["CpmmNumber"]] = (string) redGrp["MRO:TDI"];
  cpmmTdiFlag[(int)irInst["CpmmNumber"]] = (string) irGrp["MRO:TDI"];
  cpmmTdiFlag[(int)bgInst["CpmmNumber"]] = (string) bgGrp["MRO:TDI"];

  // Concatenate all summing modes into one keyword
  PvlKeyword cpmmSummingFlag("cpmmSummingFlag");
  for (int i=0; i<14; i++) {
    cpmmSummingFlag += (string) "";
  }
  cpmmSummingFlag[(int)redInst["CpmmNumber"]] = (string) redGrp["MRO:BINNING"];
  cpmmSummingFlag[(int)irInst["CpmmNumber"]] = (string) irGrp["MRO:BINNING"];
  cpmmSummingFlag[(int)bgInst["CpmmNumber"]] = (string) bgGrp["MRO:BINNING"];

  //Concatenate all the Special_Processing_Flag into one keyword
  PvlKeyword specialProcessingFlag("SpecialProcessingFlag");
  for (int i=0; i<14; i++) {
    specialProcessingFlag += (string) "";
  }
  //keyword Special_Processing_Flag may not be present so need to test
  // if not present set to NOMINAL
  if(redInst.HasKeyword("Special_Processing_Flag")) {
    specialProcessingFlag[redInst["CpmmNumber"]] = (string) redInst["Special_Processing_Flag"];
  }
  else {
    specialProcessingFlag[redInst["CpmmNumber"]] = "NOMINAL";
  }
  if(irInst.HasKeyword("Special_Processing_Flag")) {
    specialProcessingFlag[irInst["CpmmNumber"]] = (string) irInst["Special_Processing_Flag"];
  }
  else {
    specialProcessingFlag[irInst["CpmmNumber"]] = "NOMINAL";
  }
  if(bgInst.HasKeyword("Special_Processing_Flag")) {
    specialProcessingFlag[bgInst["CpmmNumber"]] = (string) bgInst["Special_Processing_Flag"];
  }
  else {
    specialProcessingFlag[bgInst["CpmmNumber"]] = "NOMINAL";
  }
  // Put them in a group
  PvlGroup mos("Mosaic");
  mos += sourceProductId;
  mos += startTime;
  mos += stopTime;
  mos += startClk;
  mos += stopClk;
  mos += cpmmTdiFlag;
  mos += cpmmSummingFlag;
  mos += specialProcessingFlag;

  // Add the group to the output cube
  Cube c;
  c.Open(ui.GetFilename("TO"),"rw");
  c.Label()->FindObject("IsisCube",Pvl::Traverse).AddGroup(mos);
  c.Close();
}
