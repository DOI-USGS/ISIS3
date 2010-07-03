#include "Isis.h"
#include "ProcessByLine.h"
#include "Camera.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

// Globals and prototypes
Camera *cam;

void doIt(Buffer &in, Buffer &out);

enum OutputType {
  Lat,
  Lon,
  Err,
  Samp,
  Line
} OutputFormat;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  ProcessByLine p;

  // Open the input cube
  Cube *icube = p.SetInputCube("FROM");
  cam = icube->Camera();
  p.SetOutputCube("TO");

  iString format = ui.GetString("FORMAT");

  if(format == "LAT") {
    OutputFormat = Lat;
  }
  else if(format == "LON") {
    OutputFormat = Lon;
  }
  else if(format == "ERR") {
    OutputFormat = Err;
  }
  else if(format == "SAMP") {
    OutputFormat = Samp;
  }
  else if(format == "LINE") {
    OutputFormat = Line;
  }

  p.StartProcess(doIt);
  p.EndProcess();
}

// Line processing routine
void doIt(Buffer &in, Buffer &out) {
  if(in.Line() == 1) {
    cam->SetBand(in.Band());
  }

  for(int samp = 0; samp < in.SampleDimension(); samp++) {
    if(!cam->SetImage((double)(samp), (double)(in.Line()))) {
      out[samp] = Null;
      continue;
    }

    if(OutputFormat == Lat) {
      out[samp] = cam->UniversalLatitude();
    }
    else if(OutputFormat == Lon) {
      out[samp] = cam->UniversalLongitude();
    }
    else {
      if(!cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude())) {
        out[samp] = Null;
        continue;
      }


      if(OutputFormat == Samp) {
        out[samp] = cam->Sample();
      }
      else if(OutputFormat == Line) {
        out[samp] = cam->Line();
      }
      else {
        double deltaS = cam->Sample() - samp;
        double deltaL = cam->Line() - in.Line();
        out[samp] = pow(deltaS * deltaS + deltaL * deltaL, 0.5);
      }

    }
  }
}
