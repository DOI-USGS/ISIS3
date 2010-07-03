#include "Isis.h"
#include "Camera.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "ProjectionFactory.h"

using namespace std; 
using namespace Isis;

// Global variables
Cube *icube;
Camera *cam;
Projection *proj;
double minlat; 
double maxlat; 
double minlon;
double maxlon;
int lastBand;

void camtrim (Buffer &in, Buffer &out);

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and get the camera model
  icube = p.SetInputCube("FROM");
  cam = icube->Camera();

  // Create the output cube
  p.SetOutputCube ("TO");

  // Get the lat/lon range to trim
  UserInterface &ui = Application::GetUserInterface();
  minlat = ui.GetDouble("MINLAT");
  maxlat = ui.GetDouble("MAXLAT");
  minlon = ui.GetDouble("MINLON");
  maxlon = ui.GetDouble("MAXLON");
  
  // Get map projection to determine what type of 
  // lat/lons the user wants
  if (ui.WasEntered("MAP")) {
    Pvl lab;
    lab.Read(ui.GetFilename("MAP"));
    proj = ProjectionFactory::Create(lab);

    // add mapping to print.prt
    PvlGroup mapping = proj->Mapping(); 
    Application::Log(mapping); 
  }
  else {
    proj = NULL;
  }

  // Start the processing
  lastBand = 0;
  p.StartProcess(camtrim);
  p.EndProcess();
}

// Line processing routine
void camtrim (Buffer &in, Buffer &out) {
  // See if there is a change in band which would change the camera model
  if (in.Band() != lastBand) {
    lastBand = in.Band();
    cam->SetBand(icube->PhysicalBand(lastBand));
  }

  // Loop for each pixel in the line. 
  double samp,lat,lon;
  double line = in.Line();
  for (int i=0; i<in.size(); i++) {
    samp = in.Sample(i);
    cam->SetImage(samp,line);
    if (cam->HasSurfaceIntersection()) {
      lat = cam->UniversalLatitude();
      lon = cam->UniversalLongitude();
      if (proj != NULL) {
        proj->SetUniversalGround(lat,lon);
        lat = proj->Latitude();
        lon = proj->Longitude();
      }
      // Pixel is outside range
      if ((lat < minlat) || (lat > maxlat) || 
          (lon < minlon) || (lon > maxlon)) {
        out[i] = NULL8;
      }
      // Pixel inside range
      else {
        out[i] = in[i];
      }
    }
    // Trim outerspace
    else {
      out[i] = NULL8;
    }
  }
}
