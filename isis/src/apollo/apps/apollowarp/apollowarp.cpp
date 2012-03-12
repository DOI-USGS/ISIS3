#include "Isis.h"
#include <fstream>
#include <iostream>
#include "ProcessRubberSheet.h"
#include "WarpTransform.h"
#include "PolynomialBivariate.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessRubberSheet p;
  UserInterface &ui = Application::GetUserInterface();

  // Open the input cube
  Cube *ipacket = p.SetInputCube ("FROM");

  // Check to see if it is an Apollo image and if the reseaus have been refined
  //    (note: a status of 'Removed' implies it is also 'Refined')
  PvlGroup &reseaus = ipacket->getGroup("Reseaus");
  string mission = (ipacket->getGroup("Instrument"))["SpacecraftName"];
  if (mission.substr(0,6) != "APOLLO") {
    string msg = "This application is for use with Apollo spacecrafts only. ";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  if ((string)reseaus["Status"] != "Refined" && (string)reseaus["Status"] != "Removed") {
    string msg = "This application can only be run after findapollorx.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  // Get the master reseau info
  PvlGroup master = Pvl(reseaus["Master"]).FindGroup("MasterReseaus");

  vector<double> inputLine,inputSample,outputLine,outputSample;
  // Setup the parameters for the transform
  for (int i=0; i<reseaus["Sample"].Size(); i++) {
    inputLine.push_back(reseaus["Line"][i]);
    inputSample.push_back(reseaus["Sample"][i]);
    outputLine.push_back(master["Line"][i]);
    outputSample.push_back(master["Sample"][i]);

    // Update the cube's reseau information
    reseaus["Line"][i] = outputLine[i];
    reseaus["Sample"][i] = outputSample[i];
  }

  // Get the final output image dimensions
  PvlGroup dimensions = Pvl(reseaus["Master"]).FindGroup("Dimensions");
  int onl = dimensions["UndistortedSamples"],
      ons = dimensions["UndistortedLines"];

  // Create the basis function for transforming
  int degree = ui.GetInteger("DEGREE");
  BasisFunction *basisLine = new PolynomialBivariate(degree);
  BasisFunction *basisSamp = new PolynomialBivariate(degree);
  bool weighted = ui.GetBoolean("WEIGHTED");

  // Set up the transform object
  WarpTransform *transform = new WarpTransform(*basisLine,*basisSamp,weighted,inputLine,inputSample,outputLine,outputSample,ipacket->getLineCount(),ipacket->getSampleCount(),onl,ons);

  // Allocate the output file, same size as input
  p.SetOutputCube ("TO",transform->OutputSamples(),transform->OutputLines(),ipacket->getBandCount());

  // Set up the interpolator
  Interpolator *interp;
  if (ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if (ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else if (ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }
  else {
    string msg = "Unknow value for INTERP [" +
                 ui.GetString("INTERP") + "]";
    throw IException(IException::Programmer, msg,_FILEINFO_);
  }

  // Warp the image
  p.StartProcess(*transform,*interp);
  p.EndProcess();

  delete transform;
  delete interp;
  delete basisLine;
  delete basisSamp;
}
