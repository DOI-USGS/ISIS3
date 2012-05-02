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

// Globals
vector<double> weight;
double scaleFactor0; // Entire scale factor that e is multiplied by
double scaleFactor; // A1 or A2 in the equation, depending on units
double dcScaleFactor;
double exposureDuration;
bool eightBitDarkCube; // Is the dark cube of type unsigned byte?
bool iof; // Determines output units (i.e. I/F or radiance)

void Calibrate(vector<Buffer *> &in, vector<Buffer *> &out);
FileName FindDarkFile(Cube *icube);
FileName FindGainFile(Cube *icube);
FileName FindShutterFile(Cube *icube);
FileName ReadWeightTable(Cube *icube);
FileName GetScaleFactorFile();
int getGainModeID(Cube *icube);
void calculateScaleFactor0(Cube *icube, Cube *gaincube);

void IsisMain() {
  // Initialize Globals
  weight.clear();
  dcScaleFactor = 0.0;
  exposureDuration = 0.0;

  UserInterface &ui = Application::GetUserInterface();

  // Set up our ProcessByLine
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");
  std::cout << "Dark File: " << FindDarkFile(icube).expanded() << std::endl;
  std::cout << "Gain File: " << FindGainFile(icube).expanded() << std::endl;
  std::cout << "Shutter File: " << FindShutterFile(icube).expanded() << std::endl;

  Isis::CubeAttributeInput inAtt1;
  FileName darkFileName = FindDarkFile(icube);
  Cube *darkcube = p.SetInputCube(darkFileName.expanded(), inAtt1);
  dcScaleFactor = darkcube->getGroup("Instrument")["PicScale"][0];

  Isis::CubeAttributeInput inAtt2;
  FileName gainFileName = FindGainFile(icube);
  Cube *gaincube = p.SetInputCube(gainFileName.expanded(), inAtt2);

  Isis::CubeAttributeInput inAtt3;
  p.SetInputCube(FindShutterFile(icube).expanded(), inAtt3, Isis::AllMatchOrOne);

  Cube *ocube = p.SetOutputCube("TO");

  (ui.GetString("UNITS") == "IOF") ? iof = true : iof = false;
  scaleFactor = ui.GetDouble("SCALE");

  if(ui.GetBoolean("BITWEIGHTING")) {
    ReadWeightTable(icube);
  }
  else {
    for(int i = 0; i < 256; i++) {
      weight.push_back(i);
    }
  }

  calculateScaleFactor0(icube, gaincube);

  exposureDuration = ((double)icube->getGroup("Instrument")["ExposureDuration"][0]) * 1000;

  if(darkcube->getPixelType() == Isis::UnsignedByte) {
    eightBitDarkCube = true;
  }
  else {
    eightBitDarkCube = false;
  }

  p.StartProcess(Calibrate);

  PvlGroup calibrationLog("RadiometricCalibration");
  calibrationLog.AddKeyword(PvlKeyword("From", (std::string)ui.GetFileName("FROM")));

  FileName shutterFileName = FindShutterFile(icube);
  calibrationLog.AddKeyword(PvlKeyword("DarkCurrentFile", darkFileName.originalPath() + "/" + darkFileName.name()));
  calibrationLog.AddKeyword(PvlKeyword("GainFile", gainFileName.originalPath() + "/" + gainFileName.name()));
  calibrationLog.AddKeyword(PvlKeyword("ShutterFile", shutterFileName.originalPath() + "/" + shutterFileName.name()));
  calibrationLog.AddKeyword(PvlKeyword("ScaleFactor", scaleFactor));
  calibrationLog.AddKeyword(PvlKeyword("OutputUnits", iof ? "I/F" : "Radiance"));

  ocube->putGroup(calibrationLog);
  Application::Log(calibrationLog);
  p.EndProcess();
}

void Calibrate(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inputFile   = *in[0];
  Buffer &darkFile    = *in[1];
  Buffer &gainFile    = *in[2];
  Buffer &shutterFile = *in[3];
  Buffer &outputFile  = *out[0];

  /*
   * Calculate this part of the radiometric correction equation:
   *
   * scaleFactor0 / (t-to)
   */
  double scale = scaleFactor0 / (exposureDuration - shutterFile[0]);

  for(int samp = 0; samp < inputFile.size(); samp++) {
    // Some shutter files are only a single sample. Others may match the number
    // of samples in the cube.
    int shutterIndex = (shutterFile.size() == 1) ? 0 : samp;

    // Don't do anything to special pixels.
    if(IsSpecial(inputFile[samp])) {
      outputFile[samp] = inputFile[samp];
      continue;
    }

    if(IsSpecial(darkFile[samp]) || IsSpecial(gainFile[samp]) || IsSpecial(shutterFile[shutterIndex])) {
      outputFile[samp] = Isis::Null;
      continue;
    }

    /*
     * Calculate this part of the equation:
     *   e = z(d - dc)
     */
    double dn = weight[(unsigned long)(inputFile[samp])];

    double dc;
    if (eightBitDarkCube) {
      dc = weight[(unsigned long)(darkFile[samp])];
    }
    else {
      dc = (1.0 / dcScaleFactor) * darkFile[samp];
    }

    double e = gainFile[samp] * (dn - dc);
    double r = e * scale;

    // The following behavior does not match the Isis 2 version of this app.
    // In the Isis 2 version, negative I/F were kept in the output, which
    // doesn't make sense.
    if(r >= 0 || !iof) {
      outputFile[samp] = r;
    }
    else {
      outputFile[samp] = Isis::Lrs;
    }
  }
}

FileName FindDarkFile(Cube *icube) {
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
  if((int)(double)icube->getGroup("Instrument")["FrameDuration"][0] == 2) frameRateId = 1;
  if((int)(double)icube->getGroup("Instrument")["FrameDuration"][0] == 8) frameRateId = 2;
  if((int)(double)icube->getGroup("Instrument")["FrameDuration"][0] == 30) frameRateId = 3;
  if((int)(double)icube->getGroup("Instrument")["FrameDuration"][0] == 60) frameRateId = 4;
  if((int)(double)icube->getGroup("Instrument")["FrameDuration"][0] == 15) frameRateId = 5;

  int exposureTypeId = (icube->getGroup("Instrument")["ExposureType"][0] == "NORMAL") ? 0 : 1;

  // We have what we need from the image label, now go through the text file that is our table line by line
  // looking for a match.
  while(darkFile.GetLine(data)) {
    data = data.Compress();

    iString mission = data.Token(" ");
    if(mission != "GALILEO") {
      continue;
    }

    iString frameMode = data.Token(" ");
    if(frameMode.at(0) != icube->getGroup("Instrument")["FrameModeId"][0].at(0)) {
      continue;
    }

    iString gainState = data.Token(" ");
    if((int)gainState != gainModeId) {
      continue;
    }

    iString frameRate = data.Token(" ");
    if(frameRateId != (int)frameRate) {
      continue;
    }

    iString tableExposureTypeId = data.Token(" ");
    if((int)tableExposureTypeId != exposureTypeId) {
      continue;
    }

    iString readout = data.Token(" ");
    if(readout.at(0) != icube->getGroup("Instrument")["ReadoutMode"][0].at(0)) {
      continue;
    }

    int minImageNum = data.Token(" ");
    int maxImageNum = data.Token(" ");

    int imageNumber = (int)((double)icube->getGroup("Instrument")["SpacecraftClockStartCount"] * 100 + 0.5);
    iString telemetry = icube->getGroup("Instrument")["TelemetryFormat"][0];
    if(imageNumber > 99757701 && imageNumber < 159999999) {
      if((telemetry == "AI8" && (gainState == "1" || gainState == "2")) ||
          (telemetry == "IM4" && (gainState == "3" || gainState == "4"))) {
        imageNumber = 160000001;
      }
      else {
        imageNumber = 1;
      }
    }

    if(imageNumber < minImageNum || imageNumber > maxImageNum) {
      continue;
    }

    // By process of elimination, we found the dark current file successfully. Return it.
    return FileName(string("$galileo/calibration/darkcurrent/") + data.Token(" ") + string(".cub"));
  }

  throw IException(IException::Unknown, "Dark current file could not be determined.", _FILEINFO_);
}

FileName FindGainFile(Cube *icube) {
  string file = "$galileo/calibration/gll_gain.sav";

  TextFile gainFile(file);
  gainFile.SetComment("C");
  iString data;

  int imageNumber = (int)((double)icube->getGroup("Instrument")["SpacecraftClockStartCount"] * 100 + 0.5);

  while(gainFile.GetLine(data)) {
    data = data.Compress();

    iString mission = data.Token(" ");
    if(mission != "GALILEO") {
      continue;
    }

    /**
     * Filter codes
     * 0=clear,1=green,2=red,3=violet,4=7560,5=9680,6=7270,7=8890
     */
    iString filter = icube->getGroup("BandBin")["FilterNumber"][0];
    if(filter != data.Token(" ")) {
      continue;
    }

    iString frameMode = data.Token(" ");
    if(frameMode.at(0) != icube->getGroup("Instrument")["FrameModeId"][0].at(0)) {
      continue;
    }

    int minImageNum = data.Token(" ");
    int maxImageNum = data.Token(" ");
    if(imageNumber < minImageNum || imageNumber > maxImageNum) {
      continue;
    }

    return FileName(string("$galileo/calibration/gain/") + data.Token(" ") + string(".cub"));
  }

  throw IException(IException::Unknown, "Gain file could not be determined.", _FILEINFO_);
}

FileName ReadWeightTable(Cube *icube) {
  string file = "$galileo/calibration/weightTables_v???.sav";

  FileName weightFile(file);
  weightFile = weightFile.highestVersion();
  Pvl weightTables(weightFile.expanded());
  iString group = iString("FrameMode") + (char)icube->getGroup("Instrument")["FrameModeId"][0].at(0);
  PvlGroup &frameGrp = weightTables.FindGroup(group);
  iString keyword = iString("GainState") + ((getGainModeID(icube) < 3) ? iString("12") : iString("34"));

  for(int i = 0; i < frameGrp[keyword].Size(); i++) {
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
  if((int)(double)icube->getGroup("Instrument")["GainModeId"][0] == 4E5) {
    gainModeId = 1;
  }
  else if((int)(double)icube->getGroup("Instrument")["GainModeId"][0] == 1E5) {
    gainModeId = 2;
  }
  else if((int)(double)icube->getGroup("Instrument")["GainModeId"][0] == 4E4) {
    gainModeId = 3;
  }
  else if((int)(double)icube->getGroup("Instrument")["GainModeId"][0] == 1E4) {
    gainModeId = 4;
  }
  else {
    throw IException(IException::Unknown,
                     "Invalid value for Gain Mode ID [" +
                     icube->getGroup("Instrument")["GainModeId"][0] +
                     "].", _FILEINFO_);
  }

  return gainModeId;
}

/*
 * Calculates scaleFactor0, which is:
 *
 *         S1       K
 *      -------- * --- (D/5.2)**2
 *         A1       Ko
 *
 * if output units are in I/F, or:
 *
 *         S2       K
 *      -------- * ---
 *         A2       Ko
 *
 * if output units are in radiance.
 */
void calculateScaleFactor0(Cube *icube, Cube *gaincube) {
  Pvl conversionFactors(GetScaleFactorFile().expanded());
  PvlKeyword fltToRef, fltToRad;

  for(int grp = 0; grp < conversionFactors.Groups(); grp++) {
    PvlGroup currGrp = conversionFactors.Group(grp);

    // Match target name
    if(currGrp.HasKeyword("TargetName")) {
      if(icube->getGroup("Archive")["CalTargetCode"][0].find(currGrp["TargetName"][0]) != 0) {
        continue;
      }
    }

    // Match MinimumEncounter
    if(currGrp.HasKeyword("MinimumTargetName")) {
      try {
        if((int)currGrp["MinimumTargetName"] >
            (int)(iString)icube->getGroup("Archive")["CalTargetCode"][0].substr(0, 2)) {
          continue;
        }
      }
      catch(IException &) {
        continue;
      }
    }

    fltToRef = currGrp["FloatToRef"];
    fltToRad = currGrp["FloatToRad"];
  }

  int filterNumber = (int)icube->getGroup("BandBin")["FilterNumber"][0];

  if(fltToRef.Size() == 0) {
    throw IException(IException::Unknown,
                     "Unable to find matching reflectance and radiance values for target [" +
                     icube->getGroup("Instrument")["TargetName"][0] + "] in [" +
                     GetScaleFactorFile().expanded() + "]",
                     _FILEINFO_);
  }

  double s1 = fltToRef[filterNumber];
  double s2 = fltToRad[filterNumber];

  if (iof) {
    Camera *cam = icube->getCamera();
    bool camSuccess = cam->SetImage(icube->getSampleCount() / 2, icube->getLineCount() / 2);

    if(!camSuccess) {
      throw IException(IException::Unknown,
                       "Unable to calculate the Solar Distance on [" +
                       icube->getFileName() + "]", _FILEINFO_);
    }


    double rsun = cam->SolarDistance() / 5.2;

    /*
     * We are calculating I/F, so scaleFactor0 is:
     *
     *         S1       K
     *      -------- * --- (D/5.2)**2
     *         A1       Ko
     */
    scaleFactor0 =
      (s1 * ((double)conversionFactors["GainRatios"][getGainModeID(icube)-1] /
             (double)conversionFactors["GainRatios"][getGainModeID(gaincube)-1]) *
       pow(rsun, 2)) / (scaleFactor);
  }
  else {
    /*
     * We are calculating radiance, so scaleFactor0 is:
     *
     *         S2       K
     *      -------- * ---
     *         A2       Ko
     */
    scaleFactor0 = (s2 / scaleFactor) *
      ((double)conversionFactors["GainRatios"][getGainModeID(icube)-1] /
       (double)conversionFactors["GainRatios"][getGainModeID(gaincube)-1]);
  }
}

FileName GetScaleFactorFile() {
  string file = "$galileo/calibration/conversionFactors_v???.sav";
  FileName scaleFactor(file);
  scaleFactor = scaleFactor.highestVersion();
  return scaleFactor;
}

FileName FindShutterFile(Cube *icube) {
  string file = "$galileo/calibration/shutter/calibration.so02";
  file += icube->getGroup("Instrument")["FrameModeId"][0].at(0);
  file += ".cub";
  FileName shutterFile(file);
  return shutterFile;
}
