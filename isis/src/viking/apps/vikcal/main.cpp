/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"
#include "SpecialPixel.h"
#include "ProcessByLine.h"
#include "UserInterface.h"
#include "Pvl.h"
#include "CalParameters.h"
#include "CubeAttribute.h"


using namespace std;
using namespace Isis;

void cal(vector<Buffer *> &in,
         vector<Buffer *> &out);

CalParameters *calParam;
static bool linear;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Get the user interface and from filename
  UserInterface &ui = Application::GetUserInterface();

  // The linear option can never be true in Isis2, if it is needed, comment out
  // the following line, and uncomment the line below it. Also, add the code
  // segment found in the CalParameters documentation to the vikcal.xml file
  linear = false;
// linear = ui.GetBoolean("LINEAR");
  const QString in = ui.GetFileName("FROM");

  calParam = new CalParameters(in);

  // Open the input cube
  Cube icube;
  icube.open(in, "r");
  Progress prog;

  // If the file has already been calibrated, throw an error
  if(icube.hasGroup("Radiometry")) {
    QString msg = "The Viking image [" + icube.fileName() + "] has already "
        "been radiometrically calibrated";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  CubeAttributeInput dcf;
  CubeAttributeInput fff;
  const QString gainFile = (QString)FileName(calParam->GainFile()).expanded();
  const QString offsetFile = (QString)FileName(calParam->OffsetFile()).expanded();

  // Setup the input cubes
  p.SetInputCube("FROM");
  p.SetInputCube(offsetFile, dcf);
  p.SetInputCube(gainFile, fff);

  // Setup the output cube
  Cube *ocube = p.SetOutputCube("TO");

  // Set up and add the radiometry group to the output cube label
  PvlGroup calgrp("Radiometry");

  calgrp.addComment("Calibration equation in vikcal");
  calgrp.addComment("DI(l,s) = (1.0/(exp*w1))*G(l,s)*(gain*DR(l,s)+DC(l,s)+offt+offc)");
  calgrp.addComment("with  w1 = w0*((dist0*dist0) / (dist1*dist1))");
  calgrp.addComment("and  offt(l,s) = A*l + B*l*l + C*s + D*l*s + E");
  calgrp += PvlKeyword("offc", toString(calParam->Offset()));
  calgrp += PvlKeyword("exp", toString(calParam->Exposure()));
  calgrp += PvlKeyword("gain", toString(calParam->Gain()));
  calgrp += PvlKeyword("DR", in);
  calgrp += PvlKeyword("DC", calParam->OffsetFile());
  calgrp += PvlKeyword("G", calParam->GainFile());

  calgrp += PvlKeyword("w0", toString(calParam->Omega0()));
  calgrp += PvlKeyword("w1", toString(calParam->Omega1()));
  calgrp += PvlKeyword("dist0", toString(calParam->Distance()));
  calgrp += PvlKeyword("dist1", toString(calParam->Dist1()));
  calgrp += PvlKeyword("1.0/exp*w1", toString(1.0 / (calParam->Exposure() * calParam->Omega1())));

  calgrp += PvlKeyword("Acoeff", toString(calParam->Acoeff()));
  calgrp += PvlKeyword("Bcoeff", toString(calParam->Bcoeff()));
  calgrp += PvlKeyword("Ccoeff", toString(calParam->Ccoeff()));
  calgrp += PvlKeyword("Dcoeff", toString(calParam->Dcoeff()));
  calgrp += PvlKeyword("Ecoeff", toString(calParam->Ecoeff()));

  ocube->putGroup(calgrp);

  // Start the calibration process
  p.StartProcess(cal);
  p.EndProcess();
}

void cal(vector<Buffer *> &in,
         vector<Buffer *> &out) {

  Buffer &inp = *in[0];      // Input Cube
  Buffer &dcf = *in[1];      // Dark Current File
  Buffer &fff = *in[2];      // Flat Field File
  Buffer &outp = *out[0];    // Output Cube


  // Loop for each pixel in the line.
  for(int i = 0; i < inp.size(); i++) {
    if(IsSpecial(inp[i])) {
      outp[i] = inp[i];
    }
    else if(IsSpecial(fff[i]) || IsSpecial(dcf[i])) {
      outp[i] = Isis::Null;
    }
    else {
      double offc = calParam->TimeBasedOffset(inp.Line(), i + 1);
      double dnraw = (inp[i] * calParam->Gain()) + dcf[i] + offc;

      // The linear option can never be true in isis2, and therefore, this
      // section of the code has not been tested.
      if(linear == true) {
        double temp = (dnraw / calParam->NormalizingPower());
        for(int i = 1; i < calParam->LinearityPower(); i++) {
          temp *= (dnraw / calParam->NormalizingPower());
        }
        dnraw = calParam->Acoeff() * dnraw + calParam->Bcoeff() * temp;
      }

      double xmlt = 1.0 / (calParam->Exposure() * calParam->Omega1());
      outp[i] = xmlt * fff[i] * dnraw;
    }
  }

}
