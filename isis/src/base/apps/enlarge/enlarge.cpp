#include "Isis.h"

#include "Enlarge.h"
#include "iException.h"
#include "ProcessRubberSheet.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessRubberSheet p;

  // Open the input cube
  Cube *icube = p.SetInputCube("FROM");

  // Input number of samples, lines, and bands
  int ins = icube->Samples();
  int inl = icube->Lines();
  int inb = icube->Bands();

  // Output samples and lines
  int ons, onl;

  // Scaling factors
  double sampleScale, lineScale;

  UserInterface &ui = Application::GetUserInterface();
  if(ui.GetString("MODE") == "SCALE") {
    // Retrieve the provided scaling factors
    sampleScale = ui.GetDouble("SSCALE");
    lineScale   = ui.GetDouble("LSCALE");

    // Calculate the output size. If there is a fractional pixel, round up
    ons = (int)ceil(ins * sampleScale);
    onl = (int)ceil(inl * lineScale);
  }
  else {
    // Retrieve the provided sample/line dimensions in the output
    ons = ui.GetInteger("ONS");
    onl = ui.GetInteger("ONL");

    // Calculate the scaling factors
    sampleScale = (double)ons / (double)ins;
    lineScale   = (double)onl / (double)inl;
  }

  // Ensure that the calculated number of output samples and lines is greater
  // than the input
  if(ons < ins || onl < inl) {
    string msg = "Number of output samples/lines must be greater than or equal";
    msg = msg + " to the input samples/lines.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  
  // Set up the interpolator
  Interpolator *interp;
  if(ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if(ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else if(ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }
  else {
    string msg = "Unknown value for INTERP [" +
                 ui.GetString("INTERP") + "]";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }
  
  // Allocate the output file, the number of bands does not change in the output
  Cube *ocube = p.SetOutputCube("TO", ons, onl, inb);

  // Set up the transform object with the calculated scale and number of
  // output pixels
  Transform *transform = new Enlarge(icube, sampleScale, lineScale);
  p.StartProcess(*transform, *interp);
  PvlGroup resultsGrp = transform->UpdateOutputLabel(ocube);
  
  p.EndProcess();

  delete transform;
  delete interp;

  // Write the results to the log
  Application::Log(resultsGrp);
}

