#include "Isis.h"

#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "ProcessImportPds.h"
#include "ProgramLauncher.h"

#include "UserInterface.h"
#include "FileName.h"
#include "IString.h"

double range(double x);

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  FileName inFile = ui.GetFileName("FROM");
  QString id;
  Pvl lab(inFile.expanded());

  if (lab.hasObject("IMAGE_MAP_PROJECTION")) {
    QString msg = "Unsupported projected file [" + inFile.expanded() + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  try {
    id = (QString) lab.findKeyword("DATA_SET_ID");
  }
  catch(IException &e) {
    QString msg = "Unable to read [DATA_SET_ID] from input file [" +
                  inFile.expanded() + "]";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  p.SetPdsFile(inFile.expanded(), "", label);
  Cube *outcube = p.SetOutputCube("TO");

  // Get user entered special pixel ranges
  if(ui.GetBoolean("SETNULLRANGE")) {
    p.SetNull(ui.GetDouble("NULLMIN"), ui.GetDouble("NULLMAX"));
  }
  if(ui.GetBoolean("SETHRSRANGE")) {
    p.SetHRS(ui.GetDouble("HRSMIN"), ui.GetDouble("HRSMAX"));
  }
  if(ui.GetBoolean("SETHISRANGE")) {
    p.SetHIS(ui.GetDouble("HISMIN"), ui.GetDouble("HISMAX"));
  }
  if(ui.GetBoolean("SETLRSRANGE")) {
    p.SetLRS(ui.GetDouble("LRSMIN"), ui.GetDouble("LRSMAX"));
  }
  if(ui.GetBoolean("SETLISRANGE")) {
    p.SetLIS(ui.GetDouble("LISMIN"), ui.GetDouble("LISMAX"));
  }

  p.SetOrganization(Isis::ProcessImport::BSQ);

  p.StartProcess();

  // Get the directory where the Kaguya MI translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Kaguya"] + "/translations/";
  Pvl inputLabel(inFile.expanded());
  Pvl *outputLabel = outcube->label();
  FileName transFile;

  // Translate the Archive group
  transFile = transDir + "kaguyamiArchive.trn";
  PvlToPvlTranslationManager archiveXlater(inputLabel, transFile.expanded());
  archiveXlater.Auto(*(outputLabel));

  // Translate the Instrument group
  transFile = transDir + "kaguyamiInstrument.trn";
  PvlToPvlTranslationManager instrumentXlater(inputLabel, transFile.expanded());
  instrumentXlater.Auto(*(outputLabel));
    //trim trailing z's from the time strings
  PvlGroup &instGroup(outputLabel->findGroup("Instrument",Pvl::Traverse));
  QString timeString;
    //StartTime
  PvlKeyword &startTimeKeyword=instGroup["StartTime"];
  timeString = startTimeKeyword[0];
  startTimeKeyword.setValue( timeString.mid(0,timeString.lastIndexOf("Z")) );
    //StartTimeRaw
  PvlKeyword &startTimeRawKeyword=instGroup["StartTimeRaw"];
  timeString = startTimeRawKeyword[0];
  startTimeRawKeyword.setValue( timeString.mid(0,timeString.lastIndexOf("Z")) );
    //StopTime
  PvlKeyword &stopTimeKeyword=instGroup["StopTime"];
  timeString = stopTimeKeyword[0];
  stopTimeKeyword.setValue( timeString.mid(0,timeString.lastIndexOf("Z")) );
    //StopTimeRaw
  PvlKeyword &stopTimeRawKeyword=instGroup["StopTimeRaw"];
  timeString = stopTimeRawKeyword[0];
  stopTimeRawKeyword.setValue( timeString.mid(0,timeString.lastIndexOf("Z")) );


  // Translate the BandBin group
  transFile = transDir + "kaguyamiBandBin.trn";
  PvlToPvlTranslationManager bandBinXlater(inputLabel, transFile.expanded());
  bandBinXlater.Auto(*(outputLabel));

  //Set up the Kernels group
  PvlGroup kern("Kernels");
  if      (lab.findKeyword("INSTRUMENT_ID")[0] == "MI-VIS") {
    if (lab.findKeyword("BASE_BAND")[0] == "MV1") {
      kern += PvlKeyword("NaifFrameCode", toString(-131331));
    }
    else if (lab.findKeyword("BASE_BAND")[0] == "MV2") {
      kern += PvlKeyword("NaifFrameCode", toString(-131332));
    }
    else if (lab.findKeyword("BASE_BAND")[0] == "MV3") {
      kern += PvlKeyword("NaifFrameCode", toString(-131333));
    }
    else if (lab.findKeyword("BASE_BAND")[0] == "MV4") {
      kern += PvlKeyword("NaifFrameCode", toString(-131334));
    }
    else if (lab.findKeyword("BASE_BAND")[0] == "MV5") {
      kern += PvlKeyword("NaifFrameCode", toString(-131335));
    }
    kern += PvlKeyword("NaifCkCode", toString(-131330));
  }
  else if (lab.findKeyword("INSTRUMENT_ID")[0] == "MI-NIR") {
    if (lab.findKeyword("BASE_BAND")[0] == "MN1") {
      kern += PvlKeyword("NaifFrameCode", toString(-131341));
    }
    else if (lab.findKeyword("BASE_BAND")[0] == "MN2") {
      kern += PvlKeyword("NaifFrameCode", toString(-131342));
    }
    else if (lab.findKeyword("BASE_BAND")[0] == "MN3") {
      kern += PvlKeyword("NaifFrameCode", toString(-131343));
    }
    else if (lab.findKeyword("BASE_BAND")[0] == "MN4") {
      kern += PvlKeyword("NaifFrameCode", toString(-131344));
    }
    kern += PvlKeyword("NaifCkCode", toString(-131340));
  }

  //At the time of this writing there was no expectation that Kaguya ever did any binning
  //  so this is check to make sure an error is thrown if an image was binned
  if (lab.findKeyword("INSTRUMENT_ID")[0] == "MI-VIS" && outcube->sampleCount() != 962 ) {
    QString msg = "Input file [" + inFile.expanded() + "]" + " appears to be binned.  Binning was "
                  "unexpected, and is unsupported by the camera model";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  if (lab.findKeyword("INSTRUMENT_ID")[0] == "MI-NIR" && outcube->sampleCount() != 320 ) {
    QString msg = "Input file [" + inFile.expanded() + "]" + " appears to be binned.  Binning was "
                  "unexpected, and is unsupported by the camera model";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  outcube->putGroup(kern);

  p.EndProcess();
}

double range(double x) {
  double a,b,c;
  b = x / 360;
  if(b > 0) {
    c = floor(b);
  }
  else {
    c = ceil(b);
  }
  a = 360 * (b - c);
  if(a < 0) {
    a = a + 360;
  }
  return a;
}
