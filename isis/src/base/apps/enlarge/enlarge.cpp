#include "Isis.h"

#include "ProcessRubberSheet.h"
#include "enlarge.h"
#include "iException.h"
#include "AlphaCube.h"
#include "SubArea.h"

using namespace std; 
using namespace Isis;

Cube cube;

void IsisMain() {
  ProcessRubberSheet p;

  // Open the input cube
  Cube *icube = p.SetInputCube ("FROM");

  // Input number of samples, lines, and bands
  int ins = icube->Samples();
  int inl = icube->Lines();
  int inb = icube->Bands();

  // Output samples and lines
  int ons;
  int onl;

  // Scaling factors
  double sscale;
  double lscale;

  UserInterface &ui = Application::GetUserInterface();
  if (ui.GetString("MODE") == "SCALE") {
    // Retrieve the provided scaling factors
    sscale = ui.GetDouble("SSCALE");
    lscale = ui.GetDouble("LSCALE");

    // Calculate the output size. If there is a fractional pixel, round up
    ons = (int)ceil (ins * sscale);
    onl = (int)ceil (inl * lscale);
  }
  else {
    // Retrieve the provided sample/line dimensions in the output
    ons = ui.GetInteger("ONS");
    onl = ui.GetInteger("ONL");

    // Calculate the scaling factors
    sscale = (double)ons / (double)ins;
    lscale = (double)onl / (double)inl;
  }

  // Ensure that the calculated number of output samples and lines is greater
  // than the input
  if (ons < ins || onl < inl) {
    string msg = "Number of output samples/lines must be greater than or equal";
    msg = msg + " to the input samples/lines.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Set up the transform object with the calculated scale and number of
  // output pixels
  Transform *transform = new Enlarge(sscale, lscale, ons, onl); 

  string from = ui.GetFilename("FROM");
  cube.Open(from);

  // Allocate the output file, the number of bands does not change in the output
  Cube *ocube = p.SetOutputCube ("TO", ons, onl, inb);

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
    string msg = "Unknown value for INTERP [" +
                 ui.GetString("INTERP") + "]";
    throw iException::Message(iException::Programmer,msg,_FILEINFO_);
  }
  p.StartProcess(*transform, *interp);

  // Construct a label with the results
  PvlGroup results("Results");
  results += PvlKeyword ("InputLines", inl);
  results += PvlKeyword ("InputSamples", ins);
  results += PvlKeyword ("StartingLine", "1");
  results += PvlKeyword ("StartingSample", "1");
  results += PvlKeyword ("EndingLine", inl);
  results += PvlKeyword ("EndingSample", ins);
  results += PvlKeyword ("LineIncrement", 1./lscale);
  results += PvlKeyword ("SampleIncrement", 1./sscale);
  results += PvlKeyword ("OutputLines", onl);
  results += PvlKeyword ("OutputSamples", ons);

  // Update the Mapping, Instrument, and AlphaCube groups in the output
  // cube label
  SubArea s;
  s.SetSubArea(cube.Lines(),cube.Samples(),1,1,cube.Lines(),cube.Samples(),
               1./lscale,1./sscale);
  s.UpdateLabel(&cube,ocube,results);

  p.EndProcess();

  delete transform;
  delete interp;

  // Write the results to the log
  Application::Log(results);
}

