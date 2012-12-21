#include "Isis.h"

#include <fstream>
#include <iostream>

#include "ControlNet.h"
#include "FileName.h"
#include "NumericalApproximation.h"
#include "ProcessRubberSheet.h"
#include "SlitherTransform.h"
#include "UserInterface.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Warp an image
  ProcessRubberSheet p;

  // Open the input cube
  Cube *icube = p.SetInputCube("FROM");

  // Get the control point file
  UserInterface &ui = Application::GetUserInterface();
  QString cfile = ui.GetFileName("CONTROL");
  ControlNet cn(cfile);

  //  Set default type to Cubic spline interpolation
  NumericalApproximation::InterpType iType(NumericalApproximation::CubicNatural);
  QString splineType = ui.GetString("SPLINE");
  if(splineType == "LINEAR") {
    iType = NumericalApproximation::Linear;
  }
  else if(splineType == "POLYNOMIAL") {
    iType = NumericalApproximation::Polynomial;
  }
  else if(splineType == "AKIMA") {
    iType = NumericalApproximation::Akima;
  }

  // Get the sample and line shifts
  double sampleOffset = ui.GetDouble("SAMPLEOFFSET");
  double lineOffset = ui.GetDouble("LINEOFFSET");

  // Set up the transform object
  SlitherTransform transform(*icube, cn, iType, iType);
  transform.addLineOffset(lineOffset);
  transform.addSampleOffset(sampleOffset);
  QString splineDir = ui.GetString("DIRECTION");
  if(splineDir == "REVERSE") {
    transform.setReverse();
  }

  //  Dump the transform statistics
  if(ui.WasEntered("RESULTS")) {
    // Get the control point file
    QString rFile = FileName(ui.GetFileName("RESULTS")).expanded();
    ofstream os;
    os.open(rFile.toAscii().data(), ios::out);
    os << "#  Slither Transform Results\n"
       << "#  RunDate: " << iTime::CurrentLocalTime() << endl
       << "#    FROM:     " << icube->getFileName() << endl
       << "#    CNETFILE: " << cfile << endl << endl;

    transform.dumpState(os);
  }

  // Allocate the output file, same size as input
  p.SetOutputCube("TO", transform.OutputSamples(),
                  transform.OutputLines(),
                  icube->getBandCount());

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
    QString msg = "Unknown value for INTERP [" +
                 ui.GetString("INTERP") + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  // Create the output file
  p.StartProcess(transform, *interp);

  // All done!!
  p.EndProcess();
  delete interp;
}

