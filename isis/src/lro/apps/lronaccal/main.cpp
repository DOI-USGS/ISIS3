/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Camera.h"
#include "iTime.h"
#include "IException.h"
#include "TextFile.h"
#include "Brick.h"
#include "Table.h"
#include "Statistics.h"
#include <fstream>
#include <vector>

using namespace std;
using namespace Isis;

// Working functions and parameters
void ResetGlobals();
void CopyCubeIntoArray(QString &fileString, vector<double> &data);
void ReadTextDataFile(QString &fileString, vector<double> &data);
void ReadTextDataFile(QString &fileString, vector<vector<double> > &data);
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

  Isis::Pvl lab(ui.GetCubeName("FROM"));
  Isis::PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);

  // Check if it is a NAC image
  QString instId = inst["InstrumentId"];
  if(instId != "NACL" && instId != "NACR") {
    QString msg = "This is not a NAC image.  lrocnaccal requires a NAC image.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // And check if it has already run through calibration
  if(lab.findObject("IsisCube").hasGroup("Radiometry")) {
    QString msg = "This image has already been calibrated";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(lab.findObject("IsisCube").hasGroup("AlphaCube")) {
    QString msg = "This application can not be run on any image that has been geometrically transformed (i.e. scaled, rotated, sheared, or reflected) or cropped.";
    throw IException(IException::User, msg, _FILEINFO_);
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
  if(iCube->statistics()->Maximum() > 1000)
    g_maskedLeftOnly = true;

  QString darkFile, flatFile, offsetFile, coefficientFile;

  if(g_masked) {
    QString maskedFile = ui.GetAsString("MASKEDFILE");

    if(maskedFile.toLower() == "default" || maskedFile.length() == 0)
      maskedFile = "$lro/calibration/" + instId + "_MaskedPixels.????.pvl";

    FileName maskedFileName(maskedFile);
    if(maskedFileName.isVersioned())
      maskedFileName = maskedFileName.highestVersion();
    if(!maskedFileName.fileExists()) {
      QString msg = maskedFile + " does not exist.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    Pvl maskedPvl(maskedFileName.expanded());
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

    for(int i = 0; i < maskedPixels.size(); i++)
      if((g_isLeftNac && toInt(maskedPixels[i]) < cutoff) || (!g_isLeftNac && toInt(maskedPixels[i]) > cutoff))
        g_maskedPixelsLeft.push_back(toInt(maskedPixels[i]));
      else
        g_maskedPixelsRight.push_back(toInt(maskedPixels[i]));
  }

  if(g_dark) {
    darkFile = ui.GetAsString("DARKFILE");

    if(darkFile.toLower() == "default" || darkFile.length() == 0) {
      darkFile = "$lro/calibration/" + instId + "_AverageDarks";
      if(g_summed)
        darkFile += "_Summed";
      darkFile += ".????.cub";
    }
    CopyCubeIntoArray(darkFile, g_averageDarkLine);
  }

  if(g_nonlinear) {
    offsetFile = ui.GetAsString("OFFSETFILE");

    if(offsetFile.toLower() == "default" || offsetFile.length() == 0) {
      offsetFile = "$lro/calibration/" + instId + "_LinearizationOffsets";
      if(g_summed)
        offsetFile += "_Summed";
      offsetFile += ".????.cub";
    }
    CopyCubeIntoArray(offsetFile, g_linearOffsetLine);

    coefficientFile = ui.GetAsString("NONLINEARITYFILE");
    if(coefficientFile.toLower() == "default" || coefficientFile.length() == 0) {
      coefficientFile = "$lro/calibration/" + instId + "_LinearizationCoefficients.????.txt";
    }
    ReadTextDataFile(coefficientFile, g_linearityCoefficients);
  }

  if(g_flatfield) {
    flatFile = ui.GetAsString("FLATFIELDFILE");

    if(flatFile.toLower() == "default" || flatFile.length() == 0) {
      flatFile = "$lro/calibration/" + instId + "_Flatfield";
      ;
      if(g_summed)
        flatFile += "_Summed";
      flatFile += ".????.cub";
    }
    CopyCubeIntoArray(flatFile, g_flatfieldLine);
  }

  if(g_radiometric) {
    QString radFile = ui.GetAsString("RADIOMETRICFILE");

    if(radFile.toLower() == "default" || radFile.length() == 0)
      radFile = "$lro/calibration/NAC_RadiometricResponsivity.????.pvl";

    FileName radFileName(radFile);
    if(radFileName.isVersioned())
      radFileName = radFileName.highestVersion();
    if(!radFileName.fileExists()) {
      QString msg = radFile + " does not exist.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    Pvl radPvl(radFileName.expanded());

    if(g_iof) {
      iTime startTime((QString) inst["StartTime"]);

      try {
        Camera *cam;
        cam = iCube->camera();
        cam->setTime(startTime);
        g_solarDistance = cam->sunToBodyDist() / KM_PER_AU;

      }
      catch(IException &e) {
        // Failed to instantiate a camera, try furnishing kernels directly
        try {

          double etStart = startTime.Et();
          // Get the distance between the Moon and the Sun at the given time in
          // Astronomical Units (AU)
          QString bspKernel1 = p.MissionData("lro", "/kernels/tspk/moon_pa_de421_1900-2050.bpc", false);
          QString bspKernel2 = p.MissionData("lro", "/kernels/tspk/de421.bsp", false);
          furnsh_c(bspKernel1.toLatin1().data());
          furnsh_c(bspKernel2.toLatin1().data());
          QString pckKernel1 = p.MissionData("base", "/kernels/pck/pck?????.tpc", true);
          QString pckKernel2 = p.MissionData("lro", "/kernels/pck/moon_080317.tf", false);
          QString pckKernel3 = p.MissionData("lro", "/kernels/pck/moon_assoc_me.tf", false);
          furnsh_c(pckKernel1.toLatin1().data());
          furnsh_c(pckKernel2.toLatin1().data());
          furnsh_c(pckKernel3.toLatin1().data());
          double sunpos[6], lt;
          spkezr_c("sun", etStart, "MOON_ME", "LT+S", "MOON", sunpos, &lt);
          g_solarDistance = vnorm_c(sunpos) / KM_PER_AU;
          unload_c(bspKernel1.toLatin1().data());
          unload_c(bspKernel2.toLatin1().data());
          unload_c(pckKernel1.toLatin1().data());
          unload_c(pckKernel2.toLatin1().data());
          unload_c(pckKernel3.toLatin1().data());
        }
        catch(IException &e) {
          QString msg = "Unable to find the necessary SPICE kernels for converting to IOF";
          throw IException(e, IException::User, msg, _FILEINFO_);
        }
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
      darkColumns += toString(g_maskedPixelsLeft[i]);
    for(unsigned int i = 0; i < g_maskedPixelsRight.size(); i++)
      darkColumns += toString(g_maskedPixelsRight[i]);
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
        calgrp += PvlKeyword("ResponsivityValue", toString(g_iofLeft));
      else
        calgrp += PvlKeyword("ResponsivityValue", toString(g_iofRight));
    }
    else {
      calgrp += PvlKeyword("RadiometricType", "AbsoluteRadiance");
      if(g_isLeftNac)
        calgrp += PvlKeyword("ResponsivityValue", toString(g_radianceLeft));
      else
        calgrp += PvlKeyword("ResponsivityValue", toString(g_radianceRight));
    }
    calgrp += PvlKeyword("SolarDistance", toString(g_solarDistance));
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

void CopyCubeIntoArray(QString &fileString, vector<double> &data) {
  Cube cube;
  FileName filename(fileString);
  if(filename.isVersioned())
    filename = filename.highestVersion();
  if(!filename.fileExists()) {
    QString msg = fileString + " does not exist.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  cube.open(filename.expanded());
  Brick brick(cube.sampleCount(), cube.lineCount(), cube.bandCount(),
              cube.pixelType());
  brick.SetBasePosition(1, 1, 1);
  cube.read(brick);
  data.clear();
  for(int i = 0; i < cube.sampleCount(); i++)
    data.push_back(brick[i]);

  fileString = filename.expanded();
}

void ReadTextDataFile(QString &fileString, vector<double> &data) {
  FileName filename(fileString);
  if(filename.isVersioned())
    filename = filename.highestVersion();
  if(!filename.fileExists()) {
    QString msg = fileString + " does not exist.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  TextFile file(filename.expanded());
  QString lineString;
  unsigned int line = 0;
  while(file.GetLine(lineString)) {
    data.push_back(toDouble(lineString.split(QRegExp("[ ,;]")).first()));
    line++;
  }
  fileString = filename.expanded();
}

void ReadTextDataFile(QString &fileString, vector<vector<double> > &data) {
  FileName filename(fileString);
  if(filename.isVersioned())
    filename = filename.highestVersion();
  if(!filename.fileExists()) {
    QString msg = fileString + " does not exist.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  TextFile file(filename.expanded());
  QString lineString;
  while(file.GetLine(lineString)) {
    vector<double> line;
    lineString = lineString.simplified().remove(QRegExp("^[ ,]*")).trimmed();

    QStringList lineTokens = lineString.split(QRegExp("[ ,]"), QString::SkipEmptyParts);
    foreach (QString value, lineTokens) {
      line.push_back(toDouble(value));
    }

    data.push_back(line);
  }

  fileString = filename.expanded();
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

      if(in[i] < MAXNONLIN) {
        if(g_summed)
          in[i] -= (1.0 / (g_linearityCoefficients[2* i ][0] * pow(g_linearityCoefficients[2* i ][1], in[i])
                           + g_linearityCoefficients[2* i ][2]) + 1.0 / (g_linearityCoefficients[2* i + 1][0] * pow(
                                 g_linearityCoefficients[2* i + 1][1], in[i]) + g_linearityCoefficients[2* i + 1][2])) / 2;
        else
          in[i] -= 1.0 / (g_linearityCoefficients[i][0] * pow(g_linearityCoefficients[i][1], in[i])
                          + g_linearityCoefficients[i][2]);
      }
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
