#include "Isis.h"
#include "Camera.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

// Global variables
Camera *cam;
Cube *icube;
double minPhase;
double maxPhase;
double minEmission;
double maxEmission;
double minIncidence;
double maxIncidence;
int lastBand;

void photrim (Buffer &in, Buffer &out);

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and get the camera model
  icube = p.SetInputCube("FROM");
  cam = icube->Camera();

  // Create the output cube
  p.SetOutputCube ("TO");

  // Get the trim angles
  UserInterface &ui = Application::GetUserInterface();

  minPhase = ui.GetDouble ("MINPHASE");
  maxPhase = ui.GetDouble ("MAXPHASE");
  minEmission = ui.GetDouble ("MINEMISSION");
  maxEmission = ui.GetDouble ("MAXEMISSION");
  minIncidence = ui.GetDouble ("MININCIDENCE");
  maxIncidence = ui.GetDouble ("MAXINCIDENCE");

  // Start the processing
  lastBand = 0;
  p.StartProcess(photrim);
  p.EndProcess();
}


// Line processing routine
void photrim (Buffer &in, Buffer &out) {
  // See if there is a change in band which would change the camera model
  if (in.Band() != lastBand) {
    lastBand = in.Band();
    cam->SetBand(icube->PhysicalBand(lastBand));
  }

  // Loop for each pixel in the line.
  double samp,phase,emission,incidence;
  double line = in.Line();
  for (int i=0; i<in.size(); i++) {
    samp = in.Sample(i);
    cam->SetImage(samp,line);
    if (cam->HasSurfaceIntersection()) {
      if (((phase = cam->PhaseAngle()) < minPhase) || (phase > maxPhase)) {
        out[i] = Isis::NULL8;
      }
      else if (((emission = cam->EmissionAngle()) < minEmission) ||
               (emission > maxEmission)) {
        out[i] = Isis::NULL8;
      }
      else if (((incidence = cam->IncidenceAngle()) < minIncidence) ||
               (incidence > maxIncidence)) {
        out[i] = Isis::NULL8;
      }
      else {
        out[i] = in[i];
      }
    }
    // Trim outerspace
    else {
      out[i] = Isis::NULL8;
    }
  }
}
