#include "Isis.h"

#include "Camera.h"
#include "ProcessRubberSheet.h"

#include "map2cam.h"

using namespace std; 
using namespace Isis;

// Global variables
Cube *mcube;
Camera *outcam;
void BandChange (const int band);

void IsisMain() {
  // Open the input camera cube that we will be matching and create
  // the camera object
  Process p;
  mcube = p.SetInputCube("MATCH");
  outcam = mcube->Camera();
  
  // Open the input projection cube and get the projection information
  ProcessRubberSheet rub;
  Cube *icube = rub.SetInputCube("FROM");
  Projection *inmap = icube->Projection();

  // Set up for rubbersheeting 
  Transform *transform = new map2cam (icube->Samples(), 
                                          icube->Lines(),
                                          inmap, 
                                          mcube->Samples(),
                                          mcube->Lines(),
                                          outcam);
  
  // Allocate the output cube but don't propogate any labels from the map
  // file. Instead propagate from the camera file
  rub.PropagateLabels(false);
  rub.SetOutputCube ("TO", transform->OutputSamples(), 
                           transform->OutputLines(), 
                           icube->Bands());
  UserInterface &ui = Application::GetUserInterface();
  rub.PropagateLabels(ui.GetFilename("MATCH"));
  
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

  // See if the camera is band dependent and account for it
  if (!outcam->IsBandIndependent()) {
    rub.BandChange(BandChange);
  }

  // Warp the cube
  rub.StartProcess(*transform, *interp);
  rub.EndProcess();

  // Cleanup
  delete transform;
  delete interp;
}

// Transform object constructor
map2cam::map2cam (const int inputSamples, const int inputLines, 
                  Projection *inmap, 
                  const int outputSamples, const int outputLines, 
                  Camera *outcam) {
  p_inputSamples = inputSamples;
  p_inputLines = inputLines;
  p_inmap = inmap;

  p_outputSamples = outputSamples;
  p_outputLines = outputLines;
  p_outcam = outcam;
} 

// Transform method mapping output line/samps to lat/lons to input line/samps
bool map2cam::Xform (double &inSample, double &inLine,
                         const double outSample, const double outLine) {
  // See if the output image coordinate converts to lat/lon
  if (!p_outcam->SetImage(outSample,outLine)) return false;
  
  // Get the universal lat/lon and see if it can be converted to input line/samp
  double lat = p_outcam->UniversalLatitude();
  double lon = p_outcam->UniversalLongitude();
  if (!p_inmap->SetUniversalGround(lat,lon)) return false;
  
  // Make sure the point is inside the input image
  if (p_inmap->WorldX() < 0.5) return false;
  if (p_inmap->WorldX() < 0.5) return false;
  if (p_inmap->WorldY() > p_inputSamples + 0.5) return false;
  if (p_inmap->WorldY() > p_inputLines + 0.5) return false;
  
  // Everything is good
  inSample = p_inmap->WorldX();
  inLine = p_inmap->WorldY();
  return true;
}
    
int map2cam::OutputSamples () const {
  return p_outputSamples;
}

int map2cam::OutputLines () const {
  return p_outputLines;
}

void BandChange (const int band) {
  outcam->SetBand(mcube->PhysicalBand(band));
}
