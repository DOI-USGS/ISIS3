#include "Isis.h"

#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "AADate.h"
#include "AAPhysicalMoon.h"
#include "ProcessImportPds.h"
#include "ProgramLauncher.h"

#include "UserInterface.h"
#include "FileName.h"

double range(double x);

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  FileName inFile = ui.GetFileName("FROM");
  iString id;
  Pvl lab(inFile.expanded());

  try {
    id = (string) lab.FindKeyword("DATA_SET_ID");
  }
  catch(IException &e) {
    string msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  p.SetPdsFile(inFile.expanded(), "", label);
  Cube *outcube = p.SetOutputCube("TO");

  p.SetOrganization(Isis::ProcessImport::BSQ);

  p.StartProcess();

  // Get the directory where the Kaguya MI translation tables are.
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Kaguya"] + "/translations/";
  Pvl inputLabel(inFile.expanded());
  Pvl *outputLabel = outcube->getLabel();
  FileName transFile;

  // Translate the Archive group
  transFile = transDir + "kaguyamiArchive.trn";
  PvlTranslationManager archiveXlater(inputLabel, transFile.expanded());
  archiveXlater.Auto(*(outputLabel));

  // Translate the Instrument group
  transFile = transDir + "kaguyamiInstrument.trn";
  PvlTranslationManager instrumentXlater(inputLabel, transFile.expanded());
  instrumentXlater.Auto(*(outputLabel));

  // Translate the BandBin group
  transFile = transDir + "kaguyamiBandBin.trn";
  PvlTranslationManager bandBinXlater(inputLabel, transFile.expanded());
  bandBinXlater.Auto(*(outputLabel));

  // At the time that this code was added, there was no camera model for the
  // Kaguya MI data. In order to be able to perform photometry on the Kaguya
  // MI data, the SubSolarLongitude and SubSolarLatitude information needed to
  // be calculated. These calculations were taken from the
  // http://www.lunar-occultations.com/rlo/ephemeris.htm web site. The formulae
  // used to do the calculations was taken from Meeus "Astronomical Algorithms"
  // (1st Ed). This code can be removed as soon as a camera model is available
  // for the Kaguya MI camera. The start and stop times for these images are
  // very close, so the start time is used to calculate the SubSolar lat/lon
  // information.
  PvlGroup &instGrp(outputLabel->FindGroup("Instrument",Pvl::Traverse));
  string starttime = "N/A";
  stringstream ss;
  int year,month,day,hour,minute;
  CAASelenographicMoonDetails detail;
  if(instGrp.HasKeyword("CorrectedStartTime")) {
    starttime = (string) instGrp["CorrectedStartTime"];
  }
  if(instGrp.HasKeyword("StartTime") && starttime == "N/A") {
    starttime = (string) instGrp["StartTime"];
  }
  if(starttime != "N/A") {
    ss << starttime.substr(0,4);
    ss >> year;
    ss.clear();
    ss << starttime.substr(5,2);
    ss >> month;
    ss.clear();
    ss << starttime.substr(8,2);
    ss >> day;
    ss.clear();
    ss << starttime.substr(11,2);
    ss >> hour;
    ss.clear();
    ss << starttime.substr(14,2);
    ss >> minute;
    ss.clear();
    CAADate cd(year, month, day, hour, minute, 0, true);
    double Jd = cd.Julian();
    detail = CAAPhysicalMoon::CalculateSelenographicPositionOfSun(Jd);
  }
  if(!(instGrp.HasKeyword("SubSolarLongitude"))) {
    if(starttime == "N/A") {
      instGrp.AddKeyword(PvlKeyword("SubSolarLongitude","N/A"));
    }
    else {
      double SubSolarLongitude = detail.l0;
      instGrp.AddKeyword(PvlKeyword("SubSolarLongitude",SubSolarLongitude));
    } 
  }
  if(!(instGrp.HasKeyword("SubSolarLatitude"))) {
    if(starttime == "N/A") {
      instGrp.AddKeyword(PvlKeyword("SubSolarLatitude","N/A"));
    }
    else {
      double SubSolarLatitude = detail.b0;
      instGrp.AddKeyword(PvlKeyword("SubSolarLatitude",SubSolarLatitude));
    }
  }
  outcube->putGroup(instGrp);

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
