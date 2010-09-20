#include "Isis.h"

#include <cmath>

#include "ProcessByLine.h"
#include "Camera.h"
#include "SpecialPixel.h"
#include "iTime.h"
#include "iException.h"
#include "Brick.h"

using namespace std; 
using namespace Isis;

// Working functions and parameters
void Mar10Cal (std::vector< Isis::Buffer* > &inCubes,
    std::vector< Isis::Buffer* > &outCubes);

Cube coCube;
Brick * coef;

bool useBlem;
bool mask;
double correctedExp;
double sunDist;
double absCoef;
double xparm;


void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and make sure it is a mariner10 file
  UserInterface & ui = Application::GetUserInterface();

  Isis::Pvl lab(ui.GetFilename("FROM"));
  Isis::PvlGroup & inst = lab.FindGroup("Instrument", Pvl::Traverse);

  std::string mission = inst["SpacecraftName"];
  if (mission != "Mariner_10") {
    string msg = "This is not a Mariner 10 image.  Mar10cal requires a Mariner 10 image.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  Cube * icube = p.SetInputCube("FROM", OneBand);

  // If it is already calibrated then complain
  if (icube->HasGroup("Radiometry")) {
    string msg = "This Mariner 10 image [" + icube->Filename() + "] has already been ";
    msg += "radiometrically calibrated";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Get label parameters we will need for calibration equation
  std::string instId = inst["InstrumentId"];
  string camera = instId.substr(instId.size()-1);

  iString filter = (string)(icube->GetGroup("BandBin"))["FilterName"];
  filter = filter.UpCase().substr(0,3);
  
  string target = inst["TargetName"];
  
  iTime startTime((string) inst["StartTime"]);

  double exposure = inst["ExposureDuration"];
  double exposureOffset = 0.0;
  if (ui.WasEntered("EXPOFF")) {
    exposureOffset = ui.GetDouble("EXPOFF");
  }
  else {
    if (camera == "A") {
      exposureOffset = 0.316;
    }
    else if (camera == "B") {
      exposureOffset = 3.060;
    }
    else {
      string msg = "Camera [" + camera + "] is not supported.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }
  correctedExp = exposure + exposureOffset;
 
  Cube * dcCube;
  if (ui.WasEntered ("DCCUBE") ) {
    dcCube = p.SetInputCube("DCCUBE");
  }
  else {
    //  Mercury Dark current
    //   ??? NOTE:  Need to find Mark's dc for venus and moon ????
    string dcFile("$mariner10/calibration/mariner_10_" + camera +
                  "_dc.cub");
    CubeAttributeInput cubeAtt;
    dcCube = p.SetInputCube(dcFile, cubeAtt);
  }


  //  Open blemish removal file
  Cube * blemCube = 0;
  useBlem = (ui.GetBoolean("BLEMMASK")) ? true : false;
  if (useBlem) {
    string blemFile("$mariner10/calibration/mariner_10_blem_" + camera + ".cub");
    CubeAttributeInput cubeAtt;
    blemCube = p.SetInputCube(blemFile, cubeAtt);
  }

  if (filter == "FAB" || filter == "WAF") {
    string msg = "Filter type [" + filter + "] is not supported at this time.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  if (ui.WasEntered ("COEFCUBE")) {
    coCube.Open(ui.GetFilename("COEFCUBE"));
  }
  else {
    Filename coFile("$mariner10/calibration/mariner_10_" + filter + "_" +
        camera + "_coef.cub");
    coCube.Open(coFile.Expanded());
  }
  coef = new Brick(icube->Samples(), 1, 6, coCube.PixelType());

  if (ui.WasEntered("ABSCOEF")) {
    absCoef = ui.GetDouble("ABSCOEF");
  }
  else {
    if (camera == "A") {
      absCoef = 16.0;
    }
    else if (camera == "B") {
      absCoef = 750.0;
    }
    else {
      string msg = "Camera [" + camera + "] is not supported.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }

  mask = ui.GetBoolean("MASK");
  xparm = ui.GetDouble("XPARM");

  // Get the distance between Mars and the Sun at the given time in
  // Astronomical Units (AU)
  Camera * cam = icube->Camera();
  bool camSuccess = cam->SetImage(icube->Samples()/2,icube->Lines()/2);
  if (!camSuccess) {
    throw iException::Message(iException::Camera,
        "Unable to calculate the Solar Distance on [" +
        icube->Filename() + "]", _FILEINFO_);
  }
  sunDist = cam->SolarDistance();

  // Setup the output cube
  Cube  *ocube = p.SetOutputCube("TO");

  // Add the radiometry group
  PvlGroup calgrp("Radiometry");

  calgrp += PvlKeyword("DarkCurrentCube", dcCube->Filename());
  if (useBlem) calgrp += PvlKeyword("BlemishRemovalCube", blemCube->Filename());
  calgrp += PvlKeyword("CoefficientCube", coCube.Filename());
  calgrp += PvlKeyword("AbsoluteCoefficient", absCoef);

  ocube->PutGroup(calgrp);
  
  // Start the line-by-line calibration sequence
  p.StartProcess(Mar10Cal);
  p.EndProcess();
}

// Line processing routine
void Mar10Cal (std::vector<Isis::Buffer *> & inCubes, 
    std::vector<Isis::Buffer *> & outCubes) {

  Buffer & in = *inCubes[0];
  Buffer & dc = *inCubes[1];
  Buffer & out = *outCubes[0];
  Buffer * blem = (useBlem) ? inCubes[2] : 0;

  coef->SetBasePosition(1, in.Line(), 1);
  coCube.Read(*coef);

  // Loop and apply calibration
  for (int samp = 0; samp < in.size(); samp++) {
    // Handle special pixels
    if (IsSpecial(in[samp])) {
      out[samp] = in[samp];
    }
    else if (IsSpecial(dc[samp]) || (useBlem && IsSpecial((*blem)[samp]))) {
      out[samp] = Isis::Null;
    }
    else if (
        IsSpecial(coef->at(coef->Index(samp+1, in.Line(), 1))),
        IsSpecial(coef->at(coef->Index(samp+1, in.Line(), 2))),
        IsSpecial(coef->at(coef->Index(samp+1, in.Line(), 3))),
        IsSpecial(coef->at(coef->Index(samp+1, in.Line(), 4))),
        IsSpecial(coef->at(coef->Index(samp+1, in.Line(), 5))),
        IsSpecial(dc.at(dc.Index(samp+1, in.Line(), 1)))) {
      out[samp] = Isis::Null;
    }
    else if (mask && (in[samp] < coef->at(coef->Index(samp+1, in.Line(), 5)) ||
          in[samp] > coef->at(coef->Index(samp+1, in.Line(), 6)))) {
      out[samp] = Isis::Null;
    }
    else if (blem && blem->at(blem->Index(samp+1, in.Line(), 1)) == Isis::Null) {
      out[samp] = Isis::Null;
    }
    else { //  OK, all pixels look good, calibrate
      // Subtract space derived DC file from M10 image
      double dcCorrected = in[samp] - dc[samp];
      if (dcCorrected <= 0.0) {
        out[samp] = Isis::Null;
      }
      else {
        double x = xparm;
        for (int iteration = 0; iteration < 9; iteration++) {

          // Ax^3 + Bx^2 + Cx + D = 0 'normal cubic equation'
          double numerator = 
            (coef->at(coef->Index(samp+1, in.Line(), 4)) * pow(x, 3)) +
            (coef->at(coef->Index(samp+1, in.Line(), 3)) * pow(x, 2)) +
            (coef->at(coef->Index(samp+1, in.Line(), 2)) * x) +
            (coef->at(coef->Index(samp+1, in.Line(), 1)) - dcCorrected);

          // Ax^2 + Bx + C = 0
          double denominator = 
            (3 * coef->at(coef->Index(samp+1, in.Line(), 4)) * pow(x, 2)) +
            (2 * coef->at(coef->Index(samp+1, in.Line(), 3)) * x) +
            coef->at(coef->Index(samp+1, in.Line(), 2));

          if (denominator != 0.0) x -= numerator / denominator;
        }

        out[samp] = (x * pow(sunDist, 2)) * absCoef / correctedExp;
      }
    }
  }
}
