#include "Isis.h"

#include "Buffer.h"
#include "Camera.h"
#include "CubeAttribute.h"
#include "IException.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void calibration(vector<Buffer *> &in,
                 vector<Buffer *> &out);

// Linearity correction
bool linear;

// calibration constants
double omegaNaught;
double sunDist;
double gainC;
double offsetC;
double deltaExpo;

// calculated values
double aCoef;
double newExpo;
double XMLT;
double w1;
double dist1;

// linear constants
double bHighEnd;
double kPowerOf;
double normalizingPower;

void IsisMain() {
  // Will be processing by line
  ProcessByLine p;

  UserInterface &ui = Application::GetUserInterface();

  Cube * incube = p.SetInputCube("FROM");
  Cube * ocube = p.SetOutputCube("TO");

  // Check for projection
  if (incube->isProjected()) {
    string msg = "The cube [" + ui.GetFilename("FROM") + "] has a projection" +
                 " and cannot be radiometrically calibrated";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // access important label objects, will be used later.
  PvlObject isiscube = incube->getLabel()->FindObject("IsisCube");
  PvlGroup instrument = isiscube.FindGroup("Instrument");
  PvlGroup archive = isiscube.FindGroup("Archive");
  PvlGroup bandbin = isiscube.FindGroup("BandBin");

  // Verify not radiometrically corrected
  if (isiscube.HasGroup("Radiometry")) {
    string msg = "Cube [" + ui.GetFilename("FROM") + "] has already been" +
                 " radiometrically corrected";
    throw IException(IException::User,msg,_FILEINFO_);
  }

  // Verify Voyager spacecraft and get number, 1 or 2
  string scNumber = instrument["SpacecraftName"][0];
  if (scNumber != "VOYAGER_1" && scNumber != "VOYAGER_2") {
    string msg = "The cube [" + ui.GetFilename("FROM") + "] does not appear" +
                 " to be a Voyager image";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  scNumber = scNumber[8];

  // Open calibration file to find constants and files
  Pvl calibra(Filename("$voyager" + scNumber +
                           "/calibration/voycal.pvl").Expanded());
  PvlObject calib;
  try {
    // Search voycal.pvl for appropriate object
    PvlObject &spacecraft = calibra.FindObject(instrument.FindKeyword("SpacecraftName")[0]);
    PvlObject &inst = spacecraft.FindObject(instrument.FindKeyword("InstrumentId")[0]);
    PvlObject &smode = inst.FindObject("ShutterMode" + instrument.FindKeyword("CameraState2")[0]);
    PvlObject &mphase = smode.FindObject(archive.FindKeyword("MissionPhaseName")[0]);
    PvlObject &cstate1 = mphase.FindObject("ScanRate" + instrument.FindKeyword("CameraState1")[0]);
    string filter = bandbin["FilterName"][0] + "_" + bandbin["FilterNumber"][0];
    calib = cstate1.FindObject(filter);
  }
  catch (IException &e) {
    string msg = "Could not find match in [voycal.pvl] calibration file,";
    msg += " the error was: ";
    msg += e.what();
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  // Get appropriate calibration files
  CubeAttributeInput in1;
  p.SetInputCube(Filename("$voyager" + scNumber + "/calibration/" +
                          (string)calib["OffsetCorrectionFile"]).Expanded(), in1);
  CubeAttributeInput in2;
  p.SetInputCube(Filename("$voyager" + scNumber + "/calibration/" +
                          (string)calib["GainCorrectionFile"]).Expanded(), in2);

  // Constants from voycal.pvl for correction
  omegaNaught = calib["OmegaNaught"];
  sunDist = calib["SunDistance"];
  gainC = calib["GainCorrection"];
  offsetC = calib["OffsetCorrection"];
  deltaExpo = calib["DeltaExposureTime"];

  // If we are doing a linear correction as well, go here
  linear = ui.GetBoolean("LINEAR");
  if (linear) {
    Pvl linearity(Filename("$voyager" + scNumber +
                     "/calibration/voylin.pvl").Expanded());
    PvlObject lin;
    try {
      // Search voylin.pvl for appropriate object
      PvlObject &spacecraft = linearity.FindObject(instrument.FindKeyword("SpacecraftName")[0]);
      PvlObject &inst = spacecraft.FindObject(instrument.FindKeyword("InstrumentId")[0]);
      PvlObject &smode = inst.FindObject("ShutterMode" + instrument.FindKeyword("CameraState2")[0]);
      PvlObject &mphase = smode.FindObject(archive.FindKeyword("MissionPhaseName")[0]);
      PvlObject &cstate1 = mphase.FindObject("ScanRate" + instrument.FindKeyword("CameraState1")[0]);
      string filter = bandbin["FilterName"][0] + "_" + bandbin["FilterNumber"][0];
      lin = cstate1.FindObject(filter);
    }
    catch (IException &e) {
      string msg = "Could not find match in [voylin.pvl] calibration file,";
      msg += " the error was: ";
      msg += e.what();
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    // Calculated constant for linear correction
    aCoef = ((double)lin["NormalizingPower"] - (double)lin["B_HighEndNon-LinearityCorrection"]) /
            (double)lin["NormalizingPower"];

    // Constants from file for linear correction
    bHighEnd = (double)lin["B_HighEndNon-LinearityCorrection"];
    kPowerOf = (double)lin["K_PowerOfNon-Linearity"];
    normalizingPower = (double)lin["NormalizingPower"];
  }

  // Other calculated constants for all corrections
  // Modified exposure duration
  newExpo = ((double)instrument["ExposureDuration"] * 1000) + (double)calib["DeltaExposureTime"];

  // Camera for sun position/distance, so we have to have spice data
  Camera * cam = incube->getCamera();
  double sunPos[3];
  cam->SunPosition(sunPos);
  // Magnitude of sun vector ||s||
  dist1 = sqrt(sunPos[0]*sunPos[0] + sunPos[1]*sunPos[1] + sunPos[2]*sunPos[2]);

  w1 = (omegaNaught/1000) * ((sunDist * sunDist) / (dist1 * dist1));

  XMLT = 1.0/(newExpo * w1);

  PvlGroup calgrp("Radiometry");
  // Regular calibration equation and constants
  calgrp.AddComment("Calibration equation in voycal:");
  calgrp.AddComment("OUT(i,j) = (IN(i,j)*GAIN)+DCF(i,j)*XMLT*FFF(i,j)");
  calgrp.AddComment("XMLT = 1.0/(EXPO*W1)");
  calgrp.AddComment("EXPO = EXPODUR + DELTAEXPO");
  calgrp.AddComment("W1 = (W0/1000)*(SUNDIST^2/CALCDIST^2)");
  calgrp.AddComment("DCF = OffsetCorrectionFile, FFF = GainCorrectionFile");
  calgrp.AddComment("IN = InputCube, GAIN = GainCorrection");
  calgrp.AddKeyword(calib[0]);
  calgrp.AddKeyword(calib[1]);
  calgrp.AddKeyword(calib[2]);
  calgrp.AddKeyword(calib[3]);
  calgrp.AddKeyword(calib[4]);
  calgrp.AddKeyword(calib[5]);
  calgrp.AddKeyword(calib[6]);
  calgrp.AddKeyword(PvlKeyword("CalcSunDistance",dist1));
  calgrp.AddKeyword(instrument["ExposureDuration"]);
  calgrp.AddKeyword(PvlKeyword("XMLT",XMLT));
  calgrp.AddKeyword(PvlKeyword("Omega_W1",w1));
  calgrp.AddKeyword(PvlKeyword("CalcExpoDuration",newExpo));

  // Linear correction equation and constants
  if (linear) {
    PvlKeyword linearity = PvlKeyword("LinearityCorrection","True");
    linearity.AddComment("Linearity correction equation:");
    linearity.AddComment("OUT(i,j) = LIN( (IN(i,j)*GAIN)+DCF(i,j) )*XMLT*FFF(i,j)");
    linearity.AddComment("LIN(X) = ACOEF*X+BCOEF*(X/XNORM)^KPOWER");
    linearity.AddComment("BCOEF = B_HighEndeNon-LinearityCorrection");
    linearity.AddComment("XNORM = NormalizingPower");
    linearity.AddComment("KPOWER = K_PowerOfNon-Linearity");
    calgrp.AddKeyword(linearity);
    calgrp.AddKeyword(PvlKeyword("ACoefficient",aCoef));
    calgrp.AddKeyword(PvlKeyword("B_HighEndNon-LinearityCorrection",bHighEnd));
    calgrp.AddKeyword(PvlKeyword("K_PowerOfNon-Linearity",kPowerOf));
    calgrp.AddKeyword(PvlKeyword("NormalizingPower",normalizingPower));
  }
  else {
    calgrp.AddKeyword(PvlKeyword("LinearityCorrection","False"));
  }

  // Add Radiometry group
  ocube->putGroup(calgrp);

  p.StartProcess(calibration);
  p.EndProcess();
}

// calibration method
void calibration(vector<Buffer *> &in, vector<Buffer *> &out) {

  Buffer &inp = *in[0];      // Input Cube
  Buffer &dcf = *in[1];      // Dark Current File
  Buffer &fff = *in[2];      // Flat Field File
  Buffer &outp = *out[0];    // Output Cube

  for (int i = 0; i < inp.size(); i++) {
    if(IsSpecial(inp[i])) {
      outp[i] = inp[i];
    }
    else if(IsSpecial(fff[i]) || IsSpecial(dcf[i])) {
      outp[i] = Isis::Null;
    }
    else {
      // Initial calculation
      double dnraw = (inp[i] * gainC) + dcf[i];
      // Linear option
      if (linear) {
        dnraw = aCoef * dnraw + bHighEnd * (pow((dnraw / normalizingPower), kPowerOf));
      }
      outp[i] = XMLT * fff[i] * dnraw;
    }
  }
}
