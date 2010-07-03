#include "Isis.h"
#include "CameraFactory.h"
#include "Camera.h"
#include "ProcessRubberSheet.h"
#include "iException.h"
#include "cam2cam.h"

using namespace std;
using namespace Isis;

// Global variables
void BandChange (const int band);
Camera *incam;

void IsisMain() {
  // Open the match cube and get the camera model on it
  ProcessRubberSheet m;
  Cube *mcube = m.SetInputCube("MATCH");
  Cube *ocube = m.SetOutputCube ("TO");

  // Set up the default reference band to the middle of the cube
  // If we have even bands it will be close to the middle
  int referenceBand = ocube->Bands();
  referenceBand += (referenceBand % 2);
  referenceBand /= 2;

  // See if the user wants to override the reference band
  UserInterface &ui = Application::GetUserInterface();
  if (ui.WasEntered("REFBAND")) {
    referenceBand = ui.GetInteger("REFBAND");
  }

  // Using the Camera method out of the object opack will not work, because the
  // filename required by the Camera is not passed by the process class in this
  // case.  Use the CameraFactory to create the Camera instead to get around this
  // problem.
  Camera *outcam = CameraFactory::Create(*(mcube->Label()));

  // Set the reference band we want to match
  PvlGroup instgrp = mcube->GetGroup("Instrument");
  if (!outcam->IsBandIndependent()) {
    PvlKeyword rBand("ReferenceBand",referenceBand);
    rBand.AddComment("# All bands are aligned to reference band");
    instgrp += rBand;
    mcube->PutGroup(instgrp);
    delete outcam;
    outcam = NULL;
  }

  // Only recreate the output camera if it was band dependent
  if (outcam == NULL) outcam = CameraFactory::Create(*(mcube->Label()));

  // We might need the instrument group later, so get a copy before clearing the input
  //   cubes.
  m.ClearInputCubes();

  Cube *icube = m.SetInputCube ("FROM");
  incam = icube->Camera();

  // Set up the transform object which will simply map
  // output line/samps -> output lat/lons -> input line/samps
  Transform *transform = new cam2cam (icube->Samples(),
                                          icube->Lines(),
                                          incam,
                                          ocube->Samples(),
                                          ocube->Lines(),
                                          outcam);


  // Add the reference band to the output if necessary
  ocube->PutGroup(instgrp);

  // Set up the interpolator
  Interpolator *interp = NULL;
  if (ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if (ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else if (ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }

  // See if we need to deal with band dependent camera models
  if (!incam->IsBandIndependent()) {
    m.BandChange(BandChange);
  }

  // Warp the cube
  m.StartProcess(*transform, *interp);
  m.EndProcess();

  // Cleanup
  delete transform;
  delete interp;
}

// Transform object constructor
cam2cam::cam2cam (const int inputSamples, const int inputLines,
                  Camera *incam, const int outputSamples,
                  const int outputLines, Camera *outcam) {
  p_inputSamples = inputSamples;
  p_inputLines = inputLines;
  p_incam = incam;

  p_outputSamples = outputSamples;
  p_outputLines = outputLines;
  p_outcam = outcam;
}

// Transform method mapping output line/samps to lat/lons to input line/samps
bool cam2cam::Xform (double &inSample, double &inLine,
                         const double outSample, const double outLine) {
  // See if the output image coordinate converts to lat/lon
  if (!p_outcam->SetImage(outSample,outLine)) return false;

  // Get the universal lat/lon and see if it can be converted to input line/samp
  double lat = p_outcam->UniversalLatitude();
  double lon = p_outcam->UniversalLongitude();
  if (!p_incam->SetUniversalGround(lat,lon)) return false;

  // Make sure the point is inside the input image
  if (p_incam->Sample() < 0.5) return false;
  if (p_incam->Line() < 0.5) return false;
  if (p_incam->Sample() > p_inputSamples + 0.5) return false;
  if (p_incam->Line() > p_inputLines + 0.5) return false;

  // Everything is good
  inSample = p_incam->Sample();
  inLine = p_incam->Line();
  return true;
}

int cam2cam::OutputSamples () const {
  return p_outputSamples;
}

int cam2cam::OutputLines () const {
  return p_outputLines;
}

void BandChange (const int band) {
  incam->SetBand(band);
}
