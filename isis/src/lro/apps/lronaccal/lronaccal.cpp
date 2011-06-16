#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Camera.h"
#include "iTime.h"
#include "iException.h"
#include "TextFile.h"
#include "LineManager.h"
#include "Brick.h"
#include "Table.h"
#include "Statistics.h"
#include <fstream>
#include <vector>

using namespace std;
using namespace Isis;

// Working functions and parameters
void ResetGlobals();
void CopyCubeIntoArray(string &fileString, vector<double> &data);
void ReadTextDataFile(string &fileString, vector<double> &data);
void ReadTextDataFile(string &fileString, vector<vector<double> > &data);
void Calibrate(Buffer &in, Buffer &out);
void RemoveMaskedOffset(Buffer &line);
void CorrectDark(Buffer &in);
void CorrectNonlinearity(Buffer &in);
void CorrectFlatfield(Buffer &in);
void RadiometricCalibration(Buffer &in);

#define LINE_SIZE 5064
#define MAXNONLIN 600
#define SOLAR_RADIUS 695500
#define KM_PER_AU 149597871
#define MASKED_PIXEL_VALUES 8

vector<int> g_maskedPixelsLeft, g_maskedPixelsRight;

double g_radianceLeft, g_radianceRight, g_iofLeft, g_iofRight;

double g_exposure; // Exposure duration
double g_solarDistance; // average distance in [AU]

bool g_summed, g_masked, g_maskedLeftOnly, g_dark, g_nonlinear, g_flatfield, g_radiometric, g_iof, g_isLeftNac;

vector<double> g_averageDarkLine, g_linearOffsetLine, g_flatfieldLine;

vector<vector<double> > g_linearityCoefficients;

// Main moccal routine
void IsisMain() {
  ResetGlobals();
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and make sure it is a ctx file
  UserInterface &ui = Application::GetUserInterface();

  g_masked = ui.GetBoolean("MASKED");
  g_dark = ui.GetBoolean("DARK");
  g_nonlinear = ui.GetBoolean("NONLINEARITY");
  g_flatfield = ui.GetBoolean("FLATFIELD");
  g_radiometric = ui.GetBoolean("RADIOMETRIC");
  g_iof = (ui.GetString("RADIOMETRICTYPE") == "IOF");

  Isis::Pvl lab(ui.GetFilename("FROM"));
  Isis::PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);

  // Check if it is a NAC image
  std::string instId = inst["InstrumentId"];
  if(instId != "NACL" && instId != "NACR") {
    string msg = "This is not a NAC image.  lrocnaccal requires a NAC image.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  // And check if it has already run through calibration
  if(lab.FindObject("IsisCube").HasGroup("Radiometry")) {
    string msg = "This image has already been calibrated";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  if(lab.FindObject("IsisCube").HasGroup("AlphaCube")) {
    string msg = "This application can not be run on any image that has been geometrically transformed (i.e. scaled, rotated, sheared, or reflected) or cropped.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  if(instId == "NACL")
    g_isLeftNac = true;
  else
    g_isLeftNac = false;

  if((int) inst["SpatialSumming"] == 1)
    g_summed = false;
  else
    g_summed = true;

  g_exposure = inst["LineExposureDuration"];

  Cube *iCube = p.SetInputCube("FROM", OneBand);

  // If there is any pixel in the image with a DN > 1000
  //  then the "left" masked pixels are likely wiped out and useless
  if(iCube->getStatistics()->Maximum() > 1000)
    g_maskedLeftOnly = true;

  iString darkFile, flatFile, offsetFile, coefficientFile;

  if(g_masked) {
    iString maskedFile = ui.GetAsString("MASKEDFILE");

    if(maskedFile.Equal("Default") || maskedFile.length() == 0)
      maskedFile = "$lro/calibration/" + instId + "_MaskedPixels.????.pvl";

    Filename maskedFilename(maskedFile);
    if((maskedFilename.Expanded()).find("?") != string::npos)
      maskedFilename.HighestVersion();
    if(!maskedFilename.Exists()) {
      string msg = maskedFile + " does not exist.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    Pvl maskedPvl(maskedFilename.Expanded());
    PvlKeyword maskedPixels;
    int cutoff;
    if(g_summed) {
      maskedPixels = maskedPvl["Summed"];
      cutoff = LINE_SIZE / 4;
    }
    else {
      maskedPixels = maskedPvl["FullResolution"];
      cutoff = LINE_SIZE / 2;
    }

    for(int i = 0; i < maskedPixels.Size(); i++)
      if((g_isLeftNac && (int) maskedPixels[i] < cutoff) || (!g_isLeftNac && (int) maskedPixels[i] > cutoff))
        g_maskedPixelsLeft.push_back(maskedPixels[i]);
      else
        g_maskedPixelsRight.push_back(maskedPixels[i]);
  }

  if(g_dark) {
    darkFile = ui.GetAsString("DARKFILE");

    if(darkFile.Equal("Default") || darkFile.length() == 0) {
      darkFile = "$lro/calibration/" + instId + "_AverageDarks";
      if(g_summed)
        darkFile += "_Summed";
      darkFile += ".????.cub";
    }
    CopyCubeIntoArray(darkFile, g_averageDarkLine);
  }

  if(g_nonlinear) {
    offsetFile = ui.GetAsString("OFFSETFILE");

    if(offsetFile.Equal("Default") || offsetFile.length() == 0) {
      offsetFile = "$lro/calibration/" + instId + "_LinearizationOffsets";
      if(g_summed)
        offsetFile += "_Summed";
      offsetFile += ".????.cub";
    }
    CopyCubeIntoArray(offsetFile, g_linearOffsetLine);

    coefficientFile = ui.GetAsString("NONLINEARITYFILE");
    if(coefficientFile.Equal("Default") || coefficientFile.length() == 0) {
      coefficientFile = "$lro/calibration/" + instId + "_LinearizationCoefficients.????.txt";
    }
    ReadTextDataFile(coefficientFile, g_linearityCoefficients);
  }

  if(g_flatfield) {
    flatFile = ui.GetAsString("FLATFIELDFILE");

    if(flatFile.Equal("Default") || flatFile.length() == 0) {
      flatFile = "$lro/calibration/" + instId + "_Flatfield";
      ;
      if(g_summed)
        flatFile += "_Summed";
      flatFile += ".????.cub";
    }
    CopyCubeIntoArray(flatFile, g_flatfieldLine);
  }

  if(g_radiometric) {
    iString radFile = ui.GetAsString("RADIOMETRICFILE");

    if(radFile.Equal("Default") || radFile.length() == 0)
      radFile = "$lro/calibration/NAC_RadiometricResponsivity.????.pvl";

    Filename radFilename(radFile);
    if((radFilename.Expanded()).find("?") != string::npos)
      radFilename.HighestVersion();
    if(!radFilename.Exists()) {
      string msg = radFile + " does not exist.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    Pvl radPvl(radFilename.Expanded());

    if(g_iof) {
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
      catch(iException &e) {
        string msg = "Unable to find the necessary SPICE kernels for converting to IOF";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
      g_iofLeft = radPvl["IOF_LEFT"];
      g_iofRight = radPvl["IOF_RIGHT"];
    }
    else {
      g_radianceLeft = radPvl["Radiance_LEFT"];
      g_radianceRight = radPvl["Radiance_RIGHT"];
    }
  }

  // Setup the output cube
  Cube *ocube = p.SetOutputCube("TO");

  // Start the line-by-line calibration sequence
  p.StartProcess(Calibrate);

  PvlGroup calgrp("Radiometry");
  if(g_masked) {
    PvlKeyword darkColumns("DarkColumns");
    for(unsigned int i = 0; i < g_maskedPixelsLeft.size(); i++)
      darkColumns += g_maskedPixelsLeft[i];
    for(unsigned int i = 0; i < g_maskedPixelsRight.size(); i++)
      darkColumns += g_maskedPixelsRight[i];
    calgrp += darkColumns;
  }
  if(g_dark)
    calgrp += PvlKeyword("DarkFile", darkFile);
  if(g_nonlinear) {
    calgrp += PvlKeyword("NonlinearOffset", offsetFile);
    calgrp += PvlKeyword("LinearizationCoefficients", coefficientFile);
  }
  if(g_flatfield)
    calgrp += PvlKeyword("FlatFile", flatFile);
  if(g_radiometric) {
    if(g_iof) {
      calgrp += PvlKeyword("RadiometricType", "IOF");
      if(g_isLeftNac)
        calgrp += PvlKeyword("ResponsivityValue", g_iofLeft);
      else
        calgrp += PvlKeyword("ResponsivityValue", g_iofRight);
    }
    else {
      calgrp += PvlKeyword("RadiometricType", "AbsoluteRadiance");
      if(g_isLeftNac)
        calgrp += PvlKeyword("ResponsivityValue", g_radianceLeft);
      else
        calgrp += PvlKeyword("ResponsivityValue", g_radianceRight);
    }
    calgrp += PvlKeyword("SolarDistance", g_solarDistance);
  }
  ocube->putGroup(calgrp);
  p.EndProcess();
}

void ResetGlobals() {
  g_exposure = 1.0; // Exposure duration
  g_solarDistance = 1.01; // average distance in [AU]

  g_maskedPixelsLeft.clear();
  g_maskedPixelsRight.clear();

  g_radianceLeft = 1.0;
  g_radianceRight = 1.0;
  g_iofLeft = 1.0;
  g_iofRight = 1.0;

  g_summed = true;
  g_masked = true;
  g_dark = true;
  g_nonlinear = true;
  g_flatfield = true;
  g_radiometric = true;
  g_iof = true;
  g_isLeftNac = true;
  g_maskedLeftOnly = false;
  g_averageDarkLine.clear();
  g_linearOffsetLine.clear();
  g_flatfieldLine.clear();
  g_linearityCoefficients.clear();
}

// Line processing routine
void Calibrate(Buffer &in, Buffer &out) {
  for(int i = 0; i < in.size(); i++)
    out[i] = in[i];

  if(g_masked)
    RemoveMaskedOffset(out);

  if(g_dark)
    CorrectDark(out);

  if(g_nonlinear)
    CorrectNonlinearity(out);

  if(g_flatfield)
    CorrectFlatfield(out);

  if(g_radiometric)
    RadiometricCalibration(out);
}

void CopyCubeIntoArray(string &fileString, vector<double> &data) {
  Cube cube;
  Filename filename(fileString);
  if((filename.Expanded()).find("?") != string::npos)
    filename.HighestVersion();
  if(!filename.Exists()) {
    string msg = fileString + " does not exist.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  cube.open(filename.Expanded());
  Brick brick(cube.getSampleCount(), cube.getLineCount(), cube.getBandCount(),
              cube.getPixelType());
  brick.SetBasePosition(1, 1, 1);
  cube.read(brick);
  data.clear();
  for(int i = 0; i < cube.getSampleCount(); i++)
    data.push_back(brick[i]);

  fileString = filename.Expanded();
}

void ReadTextDataFile(string &fileString, vector<double> &data) {
  Filename filename(fileString);
  if((filename.Expanded()).find("?") != string::npos)
    filename.HighestVersion();
  if(!filename.Exists()) {
    string msg = fileString + " does not exist.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  TextFile file(filename.Expanded());
  iString lineString;
  unsigned int line = 0;
  while(file.GetLine(lineString)) {
    data.push_back((lineString.Token(" ,;")).ToDouble());
    line++;
  }
  fileString = filename.Expanded();
}

void ReadTextDataFile(string &fileString, vector<vector<double> > &data) {
  Filename filename(fileString);
  if((filename.Expanded()).find("?") != string::npos)
    filename.HighestVersion();
  if(!filename.Exists()) {
    string msg = fileString + " does not exist.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  TextFile file(filename.Expanded());
  iString lineString;
  while(file.GetLine(lineString)) {
    vector<double> line;
    lineString.ConvertWhiteSpace();
    lineString.Compress();
    lineString.TrimHead(" ,");
    while(lineString.size() > 0) {
      line.push_back((lineString.Token(" ,")).ToDouble());
    }

    data.push_back(line);
  }

  fileString = filename.Expanded();
}

void RemoveMaskedOffset(Buffer &in) {
  int numMasked = MASKED_PIXEL_VALUES;
  if(g_summed)
    numMasked /= 2;

  vector<Statistics> statsLeft(numMasked, Statistics());
  vector<Statistics> statsRight(numMasked, Statistics());

  vector<int> leftRef(numMasked, 0);
  vector<int> rightRef(numMasked, 0);

  for(unsigned int i = 0; i < g_maskedPixelsLeft.size(); i++) {
    statsLeft[g_maskedPixelsLeft[i] % numMasked].AddData(&in[g_maskedPixelsLeft[i]], 1);
    leftRef[g_maskedPixelsLeft[i] % numMasked] += g_maskedPixelsLeft[i];
  }

  for(unsigned int i = 0; i < g_maskedPixelsRight.size(); i++) {
    statsRight[g_maskedPixelsRight[i] % numMasked].AddData(&in[g_maskedPixelsRight[i]], 1);
    rightRef[g_maskedPixelsRight[i] % numMasked] += g_maskedPixelsRight[i];
  }

  // left/rightRef is the center (average) of all the masked pixels in the set
  for(int i = 0; i < numMasked; i++) {
    leftRef[i] /= statsLeft[i].TotalPixels();
    rightRef[i] /= statsRight[i].TotalPixels();
  }

  if(g_maskedLeftOnly) {
    for(int i = 0; i < in.size(); i++) {
      in[i] -= statsLeft[i % numMasked].Average();
    }
  }
  else {
    // If we are using both sides, we interpolate between them

    for(int i = 0; i < in.size(); i++) {
      in[i] -= (statsLeft[i % numMasked].Average() * (rightRef[i % numMasked] - i) + statsRight[i % numMasked].Average()
                * (i - leftRef[i % numMasked])) / (rightRef[i % numMasked] - leftRef[i % numMasked]);
    }
  }
}

void CorrectDark(Buffer &in) {
  for(int i = 0; i < in.size(); i++) {
    if(!IsSpecial(in[i]))
      in[i] -= g_averageDarkLine[i];
    else
      in[i] = Isis::Null;
  }
}

void CorrectNonlinearity(Buffer &in) {
  for(int i = 0; i < in.size(); i++) {
    if(!IsSpecial(in[i])) {
      in[i] += g_linearOffsetLine[i];

      double sign = 1.0;
      if(in[i] < 0.0) {
        sign = -1.0;
      }

      in[i] *= sign;

      if(in[i] < MAXNONLIN) {
        if(g_summed)
          in[i] -= (1.0 / (g_linearityCoefficients[2* i ][0] * pow(g_linearityCoefficients[2* i ][1], in[i])
                           + g_linearityCoefficients[2* i ][2]) + 1.0 / (g_linearityCoefficients[2* i + 1][0] * pow(
                                 g_linearityCoefficients[2* i + 1][1], in[i]) + g_linearityCoefficients[2* i + 1][2])) / 2;
        else
          in[i] -= 1.0 / (g_linearityCoefficients[i][0] * pow(g_linearityCoefficients[i][1], in[i])
                          + g_linearityCoefficients[i][2]);
      }

      in[i] *= sign;
    }
    else
      in[i] = Isis::Null;
  }
}

void CorrectFlatfield(Buffer &in) {
  for(int i = 0; i < in.size(); i++) {
    if(!IsSpecial(in[i]) && g_flatfieldLine[i] > 0)
      in[i] /= g_flatfieldLine[i];
    else
      in[i] = Isis::Null;
  }
}

void RadiometricCalibration(Buffer &in) {
  for(int i = 0; i < in.size(); i++) {
    if(!IsSpecial(in[i])) {
      in[i] /= g_exposure;
      if(g_iof) {
        if(g_isLeftNac)
          in[i] = in[i] * pow(g_solarDistance, 2) / g_iofLeft;
        else
          in[i] = in[i] * pow(g_solarDistance, 2) / g_iofRight;
      }
      else {
        if(g_isLeftNac)
          in[i] = in[i] / g_radianceLeft;
        else
          in[i] = in[i] / g_radianceRight;
      }
    }
    else
      in[i] = Isis::Null;
  }
}
