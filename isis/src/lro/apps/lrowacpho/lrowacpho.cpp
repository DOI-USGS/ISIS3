// $Id: lrowacpho.cpp,v 1.1 2010/05/18 06:38:06 kbecker Exp $
#include "Isis.h"

#include <string>
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Hillier.h"
#include "PhotometricFunction.h"
#include "Exponential.h"
#include "Pvl.h"
#include "Cube.h"

#include "PvlGroup.h"
#include "iException.h"

using namespace std;
using namespace Isis;

// Global variables
PhotometricFunction *pho;

void phoCal(Buffer &in, Buffer &out);

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Set up the input cube and get camera information
  Cube *icube = p.SetInputCube("FROM");

  // Create the output cube
  Cube *ocube = p.SetOutputCube("TO");

  // Set up the user interface
  UserInterface &ui = Application::GetUserInterface();
  // Get the name of the parameter file
  Pvl par(ui.GetFilename("PHOPAR"));

  iString algoName = PhotometricFunction::AlgorithmName(par);
  algoName.UpCase();

  if(algoName == "HILLIER") {
    pho = new Hillier(par, *icube);
  }
  else if(algoName == "EXPONENTIAL") {
    pho = new Exponential(par, *icube);
  }
  else {
    string msg = " Algorithm Name [" + algoName + "] not recognized.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  pho->SetMinimumPhaseAngle(ui.GetDouble("MINPHASE"));
  pho->SetMaximumPhaseAngle(ui.GetDouble("MAXPHASE"));
  pho->SetMinimumEmissionAngle(ui.GetDouble("MINEMISSION"));
  pho->SetMaximumEmissionAngle(ui.GetDouble("MAXEMISSION"));
  pho->SetMinimumIncidenceAngle(ui.GetDouble("MININCIDENCE"));
  pho->SetMaximumIncidenceAngle(ui.GetDouble("MAXINCIDENCE"));

  // Start the processing
  p.StartProcess(phoCal);

  PvlGroup photo("Photometry");
  pho->Report(photo);
  ocube->PutGroup(photo);
  Application::Log(photo);
  p.EndProcess();
}

/**
 * @brief Apply Hillier photometric correction
 *
 * Short function dispatched for each line to apply the Hillier photometrc
 * correction function.
 *
 * @author kbecker (2/20/2010)
 *
 * @param in Buffer containing input data
 * @param out Buffer of photometrically corrected data
 */
void phoCal(Buffer &in, Buffer &out) {

  for(int i = 0; i < in.size(); i++) {
    //  Don't correct special pixels
    if(IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      // Get correction and test for validity
      double ph = pho->Compute(in.Line(i), in.Sample(i), in.Band(i));
      out[i] = (IsSpecial(ph) ? Null : in[i] * ph);
    }
  }

  return;
}

