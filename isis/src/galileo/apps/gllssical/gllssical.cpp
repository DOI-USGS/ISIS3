#include "Isis.h"
#include "ProcessByLine.h"
#include "Buffer.h"
#include "SpecialPixel.h"
#include "TextFile.h"
#include "Camera.h"

using namespace Isis;
using namespace std;

/**
 * Shutter file:
 * The shutter on the galileo SSI camera took from 
 * about 1ms at the top of the camera to about 1.5ms 
 * at the bottom due to friction. The shutter offset file in
 * isis 2 is rotated 90 degrees.
 * 
 * Some of the values and equations in the program were verified
 *   using the book "In Orbit at Jupiter, Contributions of the Galileo
 *   Science Team," Section H Part I.
 */

vector<double> weight;
double scaleFactor0;
double a2;
double dcScaleFactor;
double exposureDuration;
double picScale;

void Calibrate1(vector<Buffer *> &in, vector<Buffer *> &out);
void Calibrate2(vector<Buffer *> &in, vector<Buffer *> &out);
Filename FindDarkFile(Cube *icube);
Filename FindGainFile(Cube *icube);
Filename FindShutterFile(Cube *icube);
Filename ReadWeightTable(Cube *icube);
Filename GetScaleFactorFile();
int getGainModeID(Cube *icube);
void calculateScaleFactor(Cube *icube, Cube *gaincube);

void IsisMain() {
  // Initialize Globals
  UserInterface &ui = Application::GetUserInterface();
  weight.clear();
  scaleFactor0 = 0.0;
  dcScaleFactor = 0.0;
  exposureDuration = 0.0;
  a2 = 0.0;
  picScale = 0.0;

  // Set up our ProcessByLine
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");
  std::cout << "Dark File: " << FindDarkFile(icube).Expanded() << std::endl;
  std::cout << "Gain File: " << FindGainFile(icube).Expanded() << std::endl;
  std::cout << "Shutter File: " << FindShutterFile(icube).Expanded() << std::endl;

  Isis::CubeAttributeInput inAtt1;
  Filename darkFilename = FindDarkFile(icube);
  Cube *darkcube = p.SetInputCube(darkFilename.Expanded(), inAtt1);
  picScale = darkcube->GetGroup("Instrument")["PicScale"][0];

  Isis::CubeAttributeInput inAtt2;
  Filename gainFilename = FindGainFile(icube);
  Cube *gaincube = p.SetInputCube(gainFilename.Expanded(), inAtt2);

  Isis::CubeAttributeInput inAtt3;
  p.SetInputCube(FindShutterFile(icube).Expanded(), inAtt3, Isis::AllMatchOrOne);

  Cube *ocube = p.SetOutputCube("TO");

  if(ui.GetBoolean("BITWEIGHTING")) {
    ReadWeightTable(icube);
  }
  else {
    for(int i = 0; i < 256; i++) {
      weight.push_back(i);
    }
  }

  calculateScaleFactor(icube, gaincube);

  exposureDuration = ((double)icube->GetGroup("Instrument")["ExposureDuration"][0]) * 1000;

  if(darkcube->PixelType() == Isis::UnsignedByte) {
    p.StartProcess(Calibrate1);
  }
  else {
    p.StartProcess(Calibrate2);
  }

  PvlGroup calibrationLog("RadiometricCalibration");
  calibrationLog.AddKeyword(PvlKeyword("From", (std::string)ui.GetFilename("FROM")));

  Filename shutterFilename = FindShutterFile(icube);
  calibrationLog.AddKeyword(PvlKeyword("DarkCurrentFile", darkFilename.OriginalPath() + "/" + darkFilename.Name()));
  calibrationLog.AddKeyword(PvlKeyword("GainFile", gainFilename.OriginalPath() + "/" + gainFilename.Name()));
  calibrationLog.AddKeyword(PvlKeyword("ShutterFile", shutterFilename.OriginalPath() + "/" + shutterFilename.Name()));

  ocube->PutGroup(calibrationLog);
  Application::Log(calibrationLog);
  p.EndProcess();
}

void Calibrate1(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inputFile   = *in[0];
  Buffer &darkFile    = *in[1];
  Buffer &gainFile    = *in[2];
  Buffer &shutterFile = *in[3];
  Buffer &outputFile  = *out[0];

  double scaleFactor = scaleFactor0 / (exposureDuration - shutterFile[0]);

  for (int samp = 0; samp < inputFile.size(); samp++) {
    if(IsSpecial(inputFile[samp])) {
      outputFile[samp] = inputFile[samp];
      continue;
    }

    if(IsSpecial(darkFile[samp]) || IsSpecial(gainFile[samp]) || IsSpecial(shutterFile[samp])) {
      outputFile[samp] = Isis::Null;
      continue;
    }

    double xdn = weight[(unsigned long)(inputFile[samp])];
    double xdc = weight[(unsigned long)(darkFile[samp])];
    double xdo = scaleFactor * gainFile[samp] * (xdn - xdc);

    if(xdo >= 0) {
      outputFile[samp] = xdo;
    }
    else {
      outputFile[samp] = Isis::Lrs;
    }
  }
}

void Calibrate2(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inputFile   = *in[0];
  Buffer &darkFile    = *in[1];
  Buffer &gainFile    = *in[2];
  Buffer &shutterFile = *in[3];
  Buffer &outputFile  = *out[0];

  double scaleFactor = scaleFactor0 / (exposureDuration - shutterFile[0]);

  for (int samp = 0; samp < inputFile.size(); samp++) {
    if(IsSpecial(inputFile[samp])) {
      outputFile[samp] = inputFile[samp];
      continue;
    }

    if(IsSpecial(darkFile[samp]) || IsSpecial(gainFile[samp]) || IsSpecial(shutterFile[samp])) {
      outputFile[samp] = Isis::Null;
      continue;
    }

    double xdn = weight[(unsigned long)(inputFile[samp])];
    double xdo = scaleFactor * gainFile[samp] * (xdn - (1.0/picScale)*darkFile[samp]); // weight of dark file DN??? See decal1

    if(xdo >= 0) {
      outputFile[samp] = xdo;
    }
    else {
      outputFile[samp] = Isis::Lrs;
    }
  }
}

Filename FindDarkFile(Cube *icube) {
  string file = "$galileo/calibration/gll_dc.sav";

  TextFile darkFile(file);
  darkFile.SetComment("C");
  iString data;

  /**
   * The dark current table requires the following information to match: 
   *   Mission,Frame Mode ID,Gain State ID,Frame Rate ID,Extended Exposure,Readout Mode,Image Number
   * So let's grab the information we need from the image labels first
   */
  int gainModeId = getGainModeID(icube);

  int frameRateId = 0;
  /**
   * Frame rate code
   * 1 = 2 1/3 (sec)
   * 2 = 8 2/3
   * 3 = 30 1/3
   * 4 = 60 2/3
   * 5 = 15 1/6
   */
  if ((int)(double)icube->GetGroup("Instrument")["FrameDuration"][0] == 2) frameRateId = 1;
  if ((int)(double)icube->GetGroup("Instrument")["FrameDuration"][0] == 8) frameRateId = 2;
  if ((int)(double)icube->GetGroup("Instrument")["FrameDuration"][0] == 30) frameRateId = 3;
  if ((int)(double)icube->GetGroup("Instrument")["FrameDuration"][0] == 60) frameRateId = 4;
  if ((int)(double)icube->GetGroup("Instrument")["FrameDuration"][0] == 15) frameRateId = 5;

  int exposureTypeId = (icube->GetGroup("Instrument")["ExposureType"][0] == "NORMAL")? 0 : 1;

  // We have what we need from the image label, now go through the text file that is our table line by line
  //   looking for a match.
  while (darkFile.GetLine(data)) {
    data = data.Compress();

    iString mission = data.Token(" ");
    if (mission != "GALILEO") {
      continue;
    }

    iString frameMode = data.Token(" ");
    if (frameMode.at(0) != icube->GetGroup("Instrument")["FrameModeId"][0].at(0)) {
      continue;
    }

    iString gainState = data.Token(" ");
    if ((int)gainState != gainModeId) {
      continue;
    }

    iString frameRate = data.Token(" ");
    if (frameRateId != (int)frameRate) {
      continue;
    }

    iString tableExposureTypeId = data.Token(" ");
    if ((int)tableExposureTypeId != exposureTypeId) {
      continue;
    }

    iString readout = data.Token(" ");
    if (readout.at(0) != icube->GetGroup("Instrument")["ReadoutMode"][0].at(0)) {
      continue;
    }

    int minImageNum = data.Token(" ");
    int maxImageNum = data.Token(" ");

    int imageNumber = (int)((double)icube->GetGroup("Instrument")["SpacecraftClockStartCount"] * 100 + 0.5);
    iString telemetry = icube->GetGroup("Instrument")["TelemetryFormat"][0];
    if (imageNumber > 99757701 && imageNumber < 159999999) {
      if ((telemetry == "AI8" && (gainState == "1" || gainState == "2")) || 
          (telemetry == "IM4" && (gainState == "3" || gainState == "4"))) {
        imageNumber = 160000001;
      }
      else {
        imageNumber = 1;
      }
    }

    if (imageNumber < minImageNum || imageNumber > maxImageNum) {
      continue;
    }

    // By process of elimination, we found the dark current file successfully. Return it.
    return Filename(string("$galileo/calibration/darkcurrent/") + data.Token(" ") + string(".cub"));
  }

  throw iException::Message(iException::Pvl, "Dark current file could not be determined.", _FILEINFO_);
}

Filename FindGainFile(Cube *icube) {
  string file = "$galileo/calibration/gll_gain.sav";

  TextFile gainFile(file);
  gainFile.SetComment("C");
  iString data;

  int imageNumber = (int)((double)icube->GetGroup("Instrument")["SpacecraftClockStartCount"] * 100 + 0.5);

  while (gainFile.GetLine(data)) {
    data = data.Compress();

    iString mission = data.Token(" ");
    if (mission != "GALILEO") {
      continue;
    }

    /**
     * Filter codes
     * 0=clear,1=green,2=red,3=violet,4=7560,5=9680,6=7270,7=8890
     */
    iString filter = icube->GetGroup("BandBin")["FilterNumber"][0];
    if (filter != data.Token(" ")) {
      continue;
    }

    iString frameMode = data.Token(" ");
    if (frameMode.at(0) != icube->GetGroup("Instrument")["FrameModeId"][0].at(0)) {
      continue;
    }

    int minImageNum = data.Token(" ");
    int maxImageNum = data.Token(" ");
    if (imageNumber < minImageNum || imageNumber > maxImageNum) {
      continue;
    }

    return Filename(string("$galileo/calibration/gain/") + data.Token(" ") + string(".cub"));
  }

  throw iException::Message(iException::Pvl, "Gain file could not be determined.", _FILEINFO_);
}

Filename ReadWeightTable(Cube *icube) {
  string file = "$galileo/calibration/weightTables_v???.sav";

  Filename weightFile(file);
  weightFile.HighestVersion();
  Pvl weightTables(weightFile.Expanded());
  iString group = iString("FrameMode") + (char)icube->GetGroup("Instrument")["FrameModeId"][0].at(0);
  PvlGroup &frameGrp = weightTables.FindGroup(group);
  iString keyword = iString("GainState") + ((getGainModeID(icube) < 3)? iString("12") : iString("34"));

  for (int i = 0; i < frameGrp[keyword].Size(); i++) {
    weight.push_back(frameGrp[keyword][i]);
  }

  return weightFile;
}

int getGainModeID(Cube *icube) {
  int gainModeId = 0;
  /**
   * Gain mode ID code
   * 1 = 400,000
   * 2 = 100,000
   * 3 = 40,000
   * 4 = 10,000
   */
  if ((int)(double)icube->GetGroup("Instrument")["GainModeId"][0] == 4E5) {
    gainModeId = 1;
  }
  else if ((int)(double)icube->GetGroup("Instrument")["GainModeId"][0] == 1E5) {
    gainModeId = 2;
  }
  else if ((int)(double)icube->GetGroup("Instrument")["GainModeId"][0] == 4E4) {
    gainModeId = 3;
  }
  else if ((int)(double)icube->GetGroup("Instrument")["GainModeId"][0] == 1E4) {
    gainModeId = 4;
  }
  else {
    throw iException::Message(iException::Pvl,
                              "Invalid value for Gain Mode ID [" + 
                              icube->GetGroup("Instrument")["GainModeId"][0] +
                              "].", _FILEINFO_);
  }   

  return gainModeId;
}

void calculateScaleFactor(Cube *icube, Cube *gaincube) {
  Pvl conversionFactors(GetScaleFactorFile().Expanded());
  PvlKeyword fltToRef, fltToRad;

  for(int grp = 0; grp < conversionFactors.Groups(); grp++) {
    PvlGroup currGrp = conversionFactors.Group(grp);

    // Match target name
    if(currGrp.HasKeyword("TargetName")) {
      if(icube->GetGroup("Instrument")["TargetName"][0].find(currGrp["TargetName"][0]) != 0) {
        continue;
      }
    }

    // Match MinimumEncounter
    if(currGrp.HasKeyword("MinimumTargetName")) {
      try {
        if((int)currGrp["MinimumTargetName"] > 
           (int)(iString)icube->GetGroup("Instrument")["TargetName"][0].substr(0,2)) {
          continue;
        }
      }
      catch(iException e) {
        e.Clear();
        continue;
      }
    }

    fltToRef = currGrp["FloatToRef"];
    fltToRad = currGrp["FloatToRad"];
  }

  int filterNumber = (int)icube->GetGroup("BandBin")["FilterNumber"][0];

  if(fltToRef.Size() == 0) {
    throw iException::Message(iException::Pvl,
                              "Unable to find matching reflectance and radiance values for target [" + 
                              icube->GetGroup("Instrument")["TargetName"][0] + "] in [" +
                              GetScaleFactorFile().Expanded() + "]",
                              _FILEINFO_);
  }

  double s1 = fltToRef[filterNumber];
  double s2 = fltToRad[filterNumber];

  Camera *cam = icube->Camera();
  bool camSuccess = cam->SetImage(icube->Samples()/2,icube->Lines()/2);

  if(!camSuccess) {
    throw iException::Message(iException::Camera, 
                              "Unable to calculate the Solar Distance on [" +
                              icube->Filename() + "]", _FILEINFO_);
  }

  double rsun = cam->SolarDistance() / 5.2;

  a2 = (s2 / s1) / pow(rsun, 2);

  scaleFactor0 = s1 * (
    (double)conversionFactors["GainRatios"][getGainModeID(icube)] / 
    (double)conversionFactors["GainRatios"][getGainModeID(gaincube)]) * pow(rsun, 2);
}

Filename GetScaleFactorFile() {
  string file = "$galileo/calibration/conversionFactors_v???.sav";
  Filename scaleFactor(file);
  scaleFactor.HighestVersion();
  return scaleFactor;
}

Filename FindShutterFile(Cube *icube) {
  string file = "$galileo/calibration/shutter/calibration.so02";
  file += icube->GetGroup("Instrument")["FrameModeId"][0].at(0);
  file += ".cub";
  Filename shutterFile(file);
  return shutterFile;
}
