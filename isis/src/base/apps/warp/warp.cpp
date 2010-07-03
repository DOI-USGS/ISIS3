#include "Isis.h"

#include <fstream>
#include <iostream>

#include "ProcessRubberSheet.h"
#include "WarpTransform.h"
#include "PolynomialBivariate.h"
#include "UserInterface.h"
#include "ControlNet.h"

using namespace std;
using namespace Isis;
  
void IsisMain() {
  // Warp an image
  ProcessRubberSheet p;

  // Get the control point file
  UserInterface &ui = Application::GetUserInterface();
  string cfile = ui.GetFilename("CONTROL");
  ControlNet cn(cfile);

  vector<double> inputLine,inputSample,outputLine,outputSample;
  for (int i=0; i<cn.Size(); i++) {
    ControlPoint cp = cn[i];
    if (cp.Size() != 2) {
      string msg = "Control points must have exactly 2 control measures";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    if (!cp.Ignore()) {
      inputLine.push_back(cp[0].Line());
      inputSample.push_back(cp[0].Sample());
      outputLine.push_back(cp[1].Line());
      outputSample.push_back(cp[1].Sample());
    }
  }

  // If there are no valid control points,
  //  throw an error
  if (inputLine.size() < 1) {
    string msg = "The specified Control Network is empty.";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  // Open the input cube
  Cube *icube = p.SetInputCube("FROM");

  // Determine the size of the output cube
  int onl,ons;
  if (ui.GetString("OSIZE") == "MATCH") {
    Cube c;
    c.Open(iString(ui.GetFilename("CUBE")),"r");
    onl = c.Lines();
    ons = c.Samples();
    c.Close();
  }
  else if (ui.GetString("OSIZE") == "COMPUTE") {
    onl = 0;
    ons = 0;
  }
  else {
    onl = ui.GetInteger("ONL");
    ons = ui.GetInteger("ONS");
  }

  // Create the basis function for transforming
  int degree = ui.GetInteger("DEGREE");
  BasisFunction *basisLine = new PolynomialBivariate(degree);
  BasisFunction *basisSamp = new PolynomialBivariate(degree);
  bool weighted = ui.GetBoolean("WEIGHTED");

  // Set up the transform object
  WarpTransform *transform = new WarpTransform(*basisLine,*basisSamp,weighted,
                             inputLine,inputSample,
                             outputLine,outputSample,
                             icube->Lines(),icube->Samples(),
                             onl,ons);

  // Allocate the output file, same size as input
  p.SetOutputCube ("TO",transform->OutputSamples(),
                   transform->OutputLines(),
                   icube->Bands());

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
    throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
  }

  p.StartProcess(*transform, *interp);
  PvlGroup results = transform->Residuals();
  Application::Log(results);
  p.EndProcess();

  delete transform;
  delete interp;
  delete basisLine;
  delete basisSamp;
}

