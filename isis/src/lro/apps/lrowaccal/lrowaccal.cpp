#include "Isis.h"

#include <vector>

#include <QDir>

#include "Brick.h"
#include "Camera.h"
#include "Constants.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "ProcessByBrick.h"
#include "SpecialPixel.h"
#include "Statistics.h"
#include "iTime.h"

using namespace Isis;
using namespace std;

#define POLAR_MODE_SAMPLES 1024
#define NO_POLAR_MODE_SAMPLES 704
#define BW_BANDS 1
#define VIS_LINES 14
#define COLOR_BANDS 5
#define UV_SAMPLES 128
#define UV_LINES 4
#define UV_BANDS 2
#define KM_PER_AU 149597871

void ResetGlobals ();
void Calibrate ( Buffer &in, Buffer &out );
void CopyCubeIntoBuffer ( string &fileString, Buffer* &data);
double min ( double a, double b );

void GetDark(string fileString, double temp, double time, Buffer* &data1, Buffer* &data2, double & temp1, double & temp2, string & file1, string & file2);
void GetMask(string &fileString, double temp, Buffer* &data);

vector<double> g_iofResponsivity;
vector<double> g_radianceResponsivity;

bool g_dark = true, g_flatfield = true, g_radiometric = true, g_iof = true, g_specpix = true;

double g_exposure; // Exposure duration
double g_solarDistance = 1.01; // average distance in [AU]
double g_startTemperature, g_endTemperature;
double g_temp1, g_temp2;

int g_numFrames;

vector<int> g_bands;

Buffer *g_darkCube1, *g_darkCube2, *g_flatCube, *g_specpixCube;

void IsisMain () {
  ResetGlobals();
  UserInterface &ui = Application::GetUserInterface();

  ProcessByBrick p;
  Cube *icube = p.SetInputCube("FROM");

  // Make sure it is a WAC cube
  Isis::PvlGroup &inst = icube->getLabel()->FindGroup("Instrument", Pvl::Traverse);
  iString instId = (string) inst["InstrumentId"];
  instId.UpCase();
  if (instId != "WAC-VIS" && instId != "WAC-UV") {
    string msg = "This program is intended for use on LROC WAC images only. [";
    msg += icube->getFilename() + "] does not appear to be a WAC image.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // And check if it has already run through calibration
  if (icube->getLabel()->FindObject("IsisCube").HasGroup("Radiometry")) {
    string msg = "This image has already been calibrated";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (icube->getLabel()->FindObject("IsisCube").HasGroup("AlphaCube")) {
    string msg = "This application can not be run on any image that has been geometrically transformed (i.e. scaled, rotated, sheared, or reflected) or cropped.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  g_dark = ui.GetBoolean("DARK");
  g_flatfield = ui.GetBoolean("FLATFIELD");
  g_radiometric = ui.GetBoolean("RADIOMETRIC");
  g_iof = (ui.GetString("RADIOMETRICTYPE") == "IOF");
  g_specpix = ui.GetBoolean("SPECIALPIXELS");

  // Determine the dark/flat files to use
  iString offset = (string) inst["BackgroundOffset"];
  iString mode = (string) inst["Mode"];
  iString instModeId = (string) inst["InstrumentModeId"];
  instModeId.UpCase();

  if (instModeId == "COLOR" && (string) inst["InstrumentId"] == "WAC-UV")
    instModeId = "UV";
  else if (instModeId == "VIS")
    instModeId = "COLOR";

  g_startTemperature = (double) inst["BeginTemperatureFpa"];
  g_endTemperature = (double) inst["EndTemperatureFpa"];

  g_numFrames = (int) inst["NumFramelets"];

  vector<string> darkFiles;
  ui.GetAsString("DARKFILE", darkFiles);
  iString flatFile = ui.GetAsString("FLATFIELDFILE");
  iString radFile = ui.GetAsString("RADIOMETRICFILE");
  iString specpixFile = ui.GetAsString("SPECIALPIXELSFILE");

  // Figure out which bands are input
  for (int i = 1; i <= icube->getBandCount(); i++) {
    g_bands.push_back(icube->getPhysicalBand(i));
  }

  Isis::PvlGroup &bandBin = icube->getLabel()->FindGroup("BandBin", Pvl::Traverse);
  iString filter = (string) bandBin["Center"][0];

  if (g_dark) {
    if (darkFiles.size() == 0 || darkFiles[0] =="Default" || darkFiles[0].length() == 0) {
      darkFiles.resize(2);
      double temp = (double) inst["MiddleTemperatureFpa"];
      double time = iTime(inst["StartTime"][0]).Et();
      string darkFile = "$lro/calibration/wac_darks/WAC_" + instModeId;
      if (instModeId == "BW")
        darkFile += "_" + filter + "_Mode" + mode;
      darkFile += "_Offset" + offset + "_*C_*T_Dark.????.cub";
      GetDark (darkFile, temp, time, g_darkCube1, g_darkCube2, g_temp1, g_temp2, darkFiles[0], darkFiles[1]);
    }
    else if (darkFiles.size() == 1) {
      CopyCubeIntoBuffer(darkFiles[0], g_darkCube1);
      g_temp1 = 0.0;
      g_darkCube2 = new Buffer(*g_darkCube1);
      g_temp2 = g_temp1;
    }
    else {
      CopyCubeIntoBuffer(darkFiles[0], g_darkCube1);
      int index = darkFiles[0].find_last_of("_");
      g_temp1 = iString(darkFiles[0].substr( darkFiles[0].find_last_of("_", index-1), index)).ToDouble();
      CopyCubeIntoBuffer(darkFiles[1], g_darkCube2);
      index = darkFiles[1].find_last_of("_");
      g_temp2 = iString(darkFiles[1].substr( darkFiles[1].find_last_of("_", index-1), index)).ToDouble();
    }
  }

  if (g_flatfield) {
    if (flatFile.Equal("Default") || flatFile.length() == 0) {
      flatFile = "$lro/calibration/wac_flats/WAC_" + instModeId;
      if (instModeId == "BW")
        flatFile += "_" + filter + "_Mode" + mode;
      flatFile += "_Flatfield.????.cub";
    }
    CopyCubeIntoBuffer(flatFile, g_flatCube);
  }

  PvlKeyword responsivity;

  if (g_radiometric) {

    Isis::PvlKeyword &bands = icube->getLabel()->FindGroup("BandBin", Pvl::Traverse).FindKeyword("FilterNumber");

    if (radFile.Equal("Default") || radFile.length() == 0)
      radFile = "$lro/calibration/WAC_RadiometricResponsivity.????.pvl";

    Filename radFilename(radFile);
    if (radFilename.IsVersioned())
      radFilename.HighestVersion();
    if (!radFilename.Exists()) {
      string msg = radFile + " does not exist.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    Pvl radPvl(radFilename.Expanded());

    if (g_iof) {
      responsivity = radPvl["IOF"];

      for (int i = 0; i < bands.Size(); i++)
        g_iofResponsivity.push_back(responsivity[(int)bands[i] - 1]);

      try {
        iTime startTime((string) inst["StartTime"]);
        double etStart = startTime.Et();
        // Get the distance between the Moon and the Sun at the given time in
        // Astronomical Units (AU)
        string bspKernel1 = p.MissionData("lro", "/kernels/tspk/moon_pa_de421_1900-2050.bpc", false);
        string bspKernel2 = p.MissionData("lro", "/kernels/tspk/de421.bsp", false);
        furnsh_c(bspKernel1.c_str());
        furnsh_c(bspKernel2.c_str());
        string pckKernel1 = p.MissionData("base", "/kernels/pck/pck?????.tpc", true);
        string pckKernel2 = p.MissionData("lro", "/kernels/pck/moon_080317.tf", false);
        string pckKernel3 = p.MissionData("lro", "/kernels/pck/moon_assoc_me.tf", false);
        furnsh_c(pckKernel1.c_str());
        furnsh_c(pckKernel2.c_str());
        furnsh_c(pckKernel3.c_str());
        double sunpos[6], lt;
        spkezr_c("sun", etStart, "MOON_ME", "LT+S", "MOON", sunpos, &lt);
        g_solarDistance = vnorm_c(sunpos) / KM_PER_AU;
        unload_c(bspKernel1.c_str());
        unload_c(bspKernel2.c_str());
        unload_c(pckKernel1.c_str());
        unload_c(pckKernel2.c_str());
        unload_c(pckKernel3.c_str());
      }
      catch (IException &e) {
        string msg = "Can not find necessary SPICE kernels for converting to IOF";
        throw IException(e, IException::User, msg, _FILEINFO_);
      }
    }
    else {
      responsivity = radPvl["Radiance"];
      for (int i = 0; i < bands.Size(); i++)
        g_radianceResponsivity.push_back(responsivity[(int)bands[i] - 1]);
    }
  }

  if (g_specpix) {
    if (specpixFile.Equal("Default") || specpixFile.length() == 0) {
      specpixFile = "$lro/calibration/wac_masks/WAC_" + instModeId;
      double temp = (double) inst["MiddleTemperatureFpa"];
      if (instModeId == "BW")
        specpixFile += "_" + filter + "_Mode" + mode;
      specpixFile += "_*C_SpecialPixels.????.cub";
      GetMask(specpixFile, temp, g_specpixCube);
    }
    else
      CopyCubeIntoBuffer(specpixFile, g_specpixCube);
  }

  if (instModeId == "BW") {
    if (mode == "1" || mode == "0")
      p.SetBrickSize(NO_POLAR_MODE_SAMPLES, VIS_LINES, (int)min(BW_BANDS, g_bands.size()));
    else
      p.SetBrickSize(POLAR_MODE_SAMPLES, VIS_LINES, (int)min(BW_BANDS, g_bands.size()));
  }
  else if (instModeId == "COLOR") {
    p.SetBrickSize(NO_POLAR_MODE_SAMPLES, VIS_LINES, (int)min(COLOR_BANDS, g_bands.size()));
  }
  else if (instModeId == "UV") {
    p.SetBrickSize(UV_SAMPLES, UV_LINES, (int)min(UV_BANDS, g_bands.size()));
  }

  g_exposure = inst["ExposureDuration"];

  Cube *ocube = p.SetOutputCube("TO");
  p.StartProcess(Calibrate);

  // Add an output group with the appropriate information
  PvlGroup calgrp("Radiometry");
  if (g_dark) {
    PvlKeyword darks("DarkFiles");
    darks.AddValue(darkFiles[0]);
    if (darkFiles.size() > 1)
      darks.AddValue(darkFiles[1]);
    calgrp += darks;
  }
  if (g_flatfield)
    calgrp += PvlKeyword("FlatFile", flatFile);
  if (g_radiometric) {
    PvlKeyword vals("ResponsivityValues");
    if (g_iof) {
      calgrp += PvlKeyword("RadiometricType", "IOF");
      for (unsigned int i=0; i< g_iofResponsivity.size(); i++)
        vals.AddValue(g_iofResponsivity[i]);
    }
    else {
      calgrp += PvlKeyword("RadiometricType", "AbsoluteRadiance");
      for (unsigned int i=0; i< g_radianceResponsivity.size(); i++)
        vals.AddValue(g_radianceResponsivity[i]);
    }
    calgrp += vals;
    calgrp += PvlKeyword("SolarDistance", g_solarDistance);
  }
  if (g_specpix)
    calgrp += PvlKeyword("SpecialPixelsFile", specpixFile);
  ocube->putGroup(calgrp);

  p.EndProcess();
}

void ResetGlobals () {
  g_iofResponsivity.clear();
  g_radianceResponsivity.clear();

  g_dark = true;
  g_flatfield = true;
  g_radiometric = true;
  g_iof = true;
  g_specpix = true;

  g_bands.clear();

  g_exposure = 1.0; // Exposure duration
  g_solarDistance = 1.01; // average distance in [AU]

  g_temp1 = 0.0;
  g_temp2 = 0.0;

  g_numFrames = 0;

  delete g_darkCube1;
  delete g_darkCube2;
  delete g_flatCube;
  delete g_specpixCube;
}

// Calibrate each framelet
void Calibrate ( Buffer &inCube, Buffer &outCube ) {
  int frameHeight = inCube.LineDimension();
  int frameSize = inCube.SampleDimension()*inCube.LineDimension();
  int frame = inCube.Line() / frameHeight;

  for (int i = 0; i < outCube.size(); i++)
    outCube[i] = inCube[i];

  if (g_dark) {
    for ( int b=0; b<inCube.BandDimension(); b++) {
      // We find the index of the corresponding dark frame band as the offset
      int offset = g_darkCube1->Index(1, frameHeight * (int) min(frame, g_darkCube1->LineDimension()/frameHeight - 1) + 1, b+1);

      for (int i = 0; i < frameSize; i++) {
        // Calculate the temperature for the current frame
        double temp = (g_endTemperature - g_startTemperature)/g_numFrames * frame + g_startTemperature;

        // Interpolate between the two darks with the current temperaturube1
        if (IsSpecial(g_darkCube1->at(offset + i)) || IsSpecial(g_darkCube2->at(offset + i)) || IsSpecial(outCube[i + b*frameSize]))
          outCube[i + b*frameSize] = Isis::Null;
        else {
          if (g_temp1 != g_temp2)
            outCube[i + b*frameSize] -= (g_darkCube1->at(offset + i) - g_darkCube2->at(offset + i))/(g_temp1-g_temp2) * (temp - g_temp2) + g_darkCube2->at(offset + i);
          else
            outCube[i+b*frameSize] -= g_darkCube1->at(offset + i);
        }
      }
    }
  }

  if (g_flatfield) {
    for ( int b=0; b<inCube.BandDimension(); b++) {
      // We find the index of the corresponding flat frame band as the offset
      int offset = g_flatCube->Index(1, frameHeight * (int) min(frame, (g_flatCube->LineDimension()-1) / frameHeight)+1, b+1);

      for (int i = 0; i < frameSize; i++) {
        if (g_flatCube->at(offset + i) <= 0 || IsSpecial(g_flatCube->at(offset + i)) || IsSpecial(outCube[i + b*frameSize]))
          outCube[i+b*frameSize] = Isis::Null;
        else
          outCube[i+b*frameSize] /= g_flatCube->at(offset + i);
      }
    }
  }

  if (g_radiometric) {
    for (int i = 0; i < outCube.size(); i++) {
      if (IsSpecial(outCube[i]))
        outCube[i] = Isis::Null;
      else {
        outCube[i] /= g_exposure;
        if (g_iof)
          outCube[i] *= pow(g_solarDistance, 2) / g_iofResponsivity[outCube.Band(i) - 1];
        else
          outCube[i] /= g_radianceResponsivity[outCube.Band(i) - 1];
      }
    }
  }

  if (g_specpix) {
    for ( int b=0; b<inCube.BandDimension(); b++) {
      // We find the index of the corresponding flat frame band as the offset
      int offset = g_specpixCube->Index(1, frameHeight * (int) min(frame, (g_specpixCube->LineDimension()-1) / frameHeight)+1, b+1);

      for (int i = 0; i < frameSize; i++) {
        if (IsSpecial(g_specpixCube->at(offset + i)))
          outCube[i+b*frameSize] = g_specpixCube->at(offset + i);
      }
    }
  }
}

void CopyCubeIntoBuffer ( string &fileString, Buffer* &data) {
  Cube cube;
  Filename filename(fileString);
  if (filename.IsVersioned())
    filename.HighestVersion();
  if (!filename.Exists()) {
    string msg = fileString + " does not exist.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  cube.open(filename.Expanded());
  Brick brick(cube.getSampleCount(), cube.getLineCount(), cube.getBandCount(), cube.getPixelType());
  brick.SetBasePosition(1, 1, 1);
  cube.read(brick);

  data = new Buffer(brick);

  fileString = filename.Expanded();
}

double min ( double a, double b ) {
  if (a < b)
    return a;
  return b;
}

void GetDark(string fileString, double temp, double time, Buffer* &data1, Buffer* &data2, double & temp1, double & temp2, string & file1, string & file2) {
  // Find the beginning and end of the "?"s in the versioned filename
  Filename filename(fileString);
  string absolutePath = filename.Expanded();
  string basename = Filename(filename.Basename()).Basename(); // We do it twice to remove the ".????.cub"

  unsigned int tempIndex = basename.find("*C");
  vector<double> temperatures;

  unsigned int timeIndex = basename.find("*T");
  vector<int> times;

  QDir dir( (QString)(iString)(absolutePath.substr(0, absolutePath.find_last_of("/"))) );

  // Loop through all files in the dir and see if they match our name
  for (unsigned int indx=0; indx < dir.count(); indx++) {
    string file = Filename( Filename(dir[indx].toStdString()).Basename()).Basename(); // We do it twice to remove ".????.cub

    size_t fileTempEndIndex = file.find("C", tempIndex);
    size_t fileTimeEndIndex = file.find("T", fileTempEndIndex+1);
    size_t fileTimeIndex = file.find_last_not_of("0123456789", fileTimeEndIndex-1) + 1;
    if (fileTempEndIndex == string::npos || fileTimeEndIndex == string::npos || fileTimeIndex == string::npos)
      continue;

    bool matches = file.substr(0, tempIndex) == basename.substr(0,tempIndex);
    matches = matches && ( file.substr(fileTempEndIndex, fileTimeIndex-fileTempEndIndex) == basename.substr(tempIndex+1, timeIndex-tempIndex-1) );
    matches = matches && ( file.substr(fileTimeEndIndex) == basename.substr(timeIndex+1) );

    if (matches) {
      // Extract all available temperatures
      Isis::iString tempStr = file.substr(tempIndex, fileTempEndIndex - tempIndex);

      if ((tempStr.length() > 0) &&
          (tempStr.find_first_not_of("0123456789.-") == string::npos)) {
        // Make sure it isn't already included, otherwise add it
        bool add = true;
        for (unsigned int j=0; j<temperatures.size(); j++)
          if (temperatures[j] == tempStr.ToDouble())
            add = false;
        if (add)
          temperatures.push_back( tempStr.ToDouble());
      }

      // Extract all available times
      Isis::iString timeStr = file.substr(fileTimeIndex, fileTimeEndIndex - fileTimeIndex);
      if ((timeStr.length() > 0) &&
          (timeStr.find_first_not_of("0123456789.-") == string::npos)) {
        // Make sure it isn't already included, otherwise add it
        bool add = true;
        for (unsigned int j=0; j<times.size(); j++)
          if (times[j] == timeStr.ToDouble())
            add = false;
        if (add)
          times.push_back( timeStr.ToInteger());
      }
    }

  }

  // Now that we have all the available temperatures, we need to find the nearest two temperatures and interpolate (or extrapolate) between them
  if (temperatures.size() == 0) {
    string msg = "No Dark files exist for these image options [" + basename + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  else if ( temperatures.size() > 2) {
    if (abs(temp - temperatures[0]) < abs(temp - temperatures[1])){
      temp1 = temperatures[0];
      temp2 = temperatures[1];
    }
    else {
      temp1 = temperatures[1];
      temp2 = temperatures[0];
    }
    for (unsigned int i=2; i<temperatures.size(); i++){
      if (abs(temp - temperatures[i]) < abs(temp - temp1)) {
        temp2 = temp1;
        temp1 = temperatures[i];
      }
      else if (abs(temp - temperatures[i]) < abs(temp - temp2)) {
        temp2 = temperatures[i];
      }
    }
  }
  else {
    temp1=temperatures[0];
    temp2=temp1;
  }

  tempIndex = fileString.find("*C");
  timeIndex = fileString.find("*T");
  // And then find the latest available time
  int bestTime = -1;
  for (unsigned int i=0; i<times.size(); i++) {
    try {
      Filename f1(fileString.substr(0, tempIndex) + iString((int)temp1) + fileString.substr(tempIndex+1, timeIndex-tempIndex-1) + iString(times[i]) + fileString.substr(timeIndex+1));
      Filename f2(fileString.substr(0, tempIndex) + iString((int)temp2) + fileString.substr(tempIndex+1, timeIndex-tempIndex-1) + iString(times[i]) + fileString.substr(timeIndex+1));


      f1.HighestVersion();
      f2.HighestVersion();

      if ( f1.Exists() && f2.Exists() && abs(times[i] - time) < abs(bestTime - time))
        bestTime = times[i];
    }
    catch ( IException e) {
      // We ignore exceptions as we expect them every time there is not a file with the given temperature and time
    }
  }

  if (bestTime == -1) {
    string msg = "No Dark files exist for these image options [" + basename + "]\n no matching times";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  file1 = fileString.substr(0, tempIndex) + iString((int)temp1) + fileString.substr(tempIndex+1, timeIndex-tempIndex-1) + iString(bestTime) + fileString.substr(timeIndex+1);
  file2 = fileString.substr(0, tempIndex) + iString((int)temp2) + fileString.substr(tempIndex+1, timeIndex-tempIndex-1) + iString(bestTime) + fileString.substr(timeIndex+1);

  CopyCubeIntoBuffer ( file1, data1 );
  CopyCubeIntoBuffer ( file2, data2 );
}

void GetMask(string &fileString, double temp, Buffer* &data) {
  // Find the beginning and end of the "?"s in the versioned filename
  Filename filename(fileString);
  string absolutePath = filename.Expanded();
  string basename = Filename(filename.Basename()).Basename(); // We do it twice to remove the ".????.cub"

  unsigned int index = basename.find_first_of("*");
  unsigned int charsAfterVersion = basename.length() - index - 1;

  vector<double> temperatures;

  QDir dir( (QString)(iString)(absolutePath.substr(0, absolutePath.find_last_of("/"))) );

  // Loop through all files in the dir and see if they match our name
  for (unsigned int indx=0; indx < dir.count(); indx++) {

    string file = Filename( Filename(dir[indx].toStdString()).Basename()).Basename(); // We do it twice to remove ".????.cub"
    bool leftSide = file.substr(0, index) == basename.substr(0, index);
    bool rightSide = ((int)file.length()-(int)charsAfterVersion) >= 0;
    if (rightSide) {
      rightSide = rightSide &&
        (file.substr(file.length()-charsAfterVersion) == basename.substr(index+1));
    }

    if (leftSide && rightSide) {

      Isis::iString version = file.substr(index, file.length()-charsAfterVersion-index);

      if ((version.length() > 0) &&
          (version.find_first_not_of("0123456789.-") == string::npos)) {
        // Make sure it isn't already included, otherwise add it
        bool add = true;
        for (unsigned int j=0; j<temperatures.size(); j++)
          if (temperatures[j] == version.ToDouble())
            add = false;
        if (add)
          temperatures.push_back( version.ToDouble());
      }
    }
  }

  // Now that we have all the available temperatures, we need to find the nearest temperature
  if (temperatures.size() == 0) {
    string msg = "No Dark files exist for these image options [" + basename + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  double bestTemp = temperatures[0];
  for (unsigned int i=1; i< temperatures.size(); i++) {
    if (abs(temp - temperatures[i]) < abs(temp - bestTemp))
      bestTemp = temperatures[i];
  }
  index = fileString.find_first_of("*");

  string file = fileString.substr(0, index) + iString((int)bestTemp) + fileString.substr(index+1);
  CopyCubeIntoBuffer ( file, data );
  fileString = file;
}

