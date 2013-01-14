#include "Isis.h"

#include <QString>

#include "Application.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "LineManager.h"
#include "ProcessByLine.h"
#include "ProgramLauncher.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void trim(Buffer &in, Buffer &out);
void getSize(Buffer &in);

double slat, elat, slon, elon;
int smallestLine, biggestLine, smallestSample, biggestSample;
Projection *proj;

void IsisMain() {
  // Get the projection
  UserInterface &ui = Application::GetUserInterface();
  Pvl pvl(ui.GetFileName("FROM"));
  proj = ProjectionFactory::CreateFromCube(pvl);

  // Determine ground range to crop and/or trim
  if(ui.WasEntered("MINLAT")) {
    slat = ui.GetDouble("MINLAT");
    elat = ui.GetDouble("MAXLAT");
    slon = ui.GetDouble("MINLON");
    elon = ui.GetDouble("MAXLON");
  }
  else if(proj->HasGroundRange()) {
    slat = proj->MinimumLatitude();
    elat = proj->MaximumLatitude();
    slon = proj->MinimumLongitude();
    elon = proj->MaximumLongitude();
  }
  else {
    string msg = "Latitude and longitude range not defined in projection";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  QString mode = ui.GetString("MODE");
  IString tempFileName;
  
  if(mode != "TRIM") {
    smallestLine = smallestSample = INT_MAX;
    biggestLine = biggestSample = -INT_MAX;

    ProcessByLine p;
    p.SetInputCube("FROM");
    p.StartProcess(getSize);
    p.EndProcess();

    int samples = biggestSample - smallestSample + 1;
    int lines = biggestLine - smallestLine + 1;

    // Run external crop
    QString cropParams = "";
    cropParams += "from=" + ui.GetFileName("FROM");
    if(mode == "CROP") {
      cropParams += " to=" + ui.GetAsString("TO");
    }
    else {
      tempFileName = FileName::createTempFile("TEMPORARYcropped.cub").name();
      cropParams += " to=" + tempFileName.ToQt();
    }

    cropParams += " sample= "   + toString(smallestSample);
    cropParams += " nsamples= " + toString(samples);
    cropParams += " line= "     + toString(smallestLine);
    cropParams += " nlines= "   + toString(lines);

    try {
      ProgramLauncher::RunIsisProgram("crop", cropParams);
    }
    catch(IException &e) {
      QString msg = "Could not execute crop with params: [" + cropParams + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    if(mode == "BOTH") {
      delete proj;
      proj = NULL;
      Pvl pvl(tempFileName.ToQt());
      proj = ProjectionFactory::CreateFromCube(pvl);
    }
  }

  // Trim image if necessary
  if(mode != "CROP") {
    ProcessByLine p;
    CubeAttributeInput att;
    if(mode == "BOTH") {
      p.SetInputCube(tempFileName.ToQt(), att);
    }
    else { //if its trim
      p.SetInputCube("FROM");
    }
    p.SetOutputCube("TO");
    p.StartProcess(trim);
    p.EndProcess();
    if(mode == "BOTH") {
      remove(tempFileName.c_str());
    }
  }
  // Add mapping to print.prt
  PvlGroup mapping = proj->Mapping();
  Application::Log(mapping);

  delete proj;
  proj = NULL;
}

// Size up the cropped area in terms of lines and samples
void getSize(Buffer &in) {
  double lat, lon;
  for(int i = 0; i < in.size(); i++) {
    proj->SetWorld((double)in.Sample(i), (double)in.Line(i));
    lat = proj->Latitude();
    lon = proj->Longitude();

    // Skip past pixels outside of lat/lon range
    if(lat < slat || lat > elat || lon < slon || lon > elon) {
      continue;
    }
    else {
      if(in.Line(i) < smallestLine) {
        smallestLine = in.Line(i);
      }
      if(in.Line(i) > biggestLine) {
        biggestLine = in.Line(i);
      }
      if(in.Sample(i) < smallestSample) {
        smallestSample = in.Sample(i);
      }
      if(in.Sample(i) > biggestSample) {
        biggestSample = in.Sample(i);
      }
    }
  }
}

// Line processing routine
void trim(Buffer &in, Buffer &out) {
  // Loop for each pixel in the line.  Find lat/lon of pixel, if outside
  // of range, set to NULL.
  double lat, lon;
  for(int i = 0; i < in.size(); i++) {
    proj->SetWorld((double)in.Sample(i), (double)in.Line(i));
    lat = proj->Latitude();
    lon = proj->Longitude();
    if(lat < slat || lat > elat || lon < slon || lon > elon) {
      out[i] = Isis::Null;
    }
    else {
      out[i] = in[i];
    }
  }
}

