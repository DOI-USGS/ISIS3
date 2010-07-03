#include <cmath>
#include "Isis.h"
#include "FileList.h"
#include "Cube.h"
#include "Process.h"
#include "Camera.h"
#include "Pvl.h"
#include "Statistics.h"

using namespace std; 
using namespace Isis;

 template <typename T> inline T MIN(const T &A, const T &B) {
   if ( A < B ) { return (A); }
   else         { return (B); }
 }

 template <typename T> inline T MAX(const T &A, const T &B) {
   if ( A > B ) { return (A); }
   else         { return (B); }
 }

inline double SetFloor(double value, const int precision) {
    double scale = pow(10.0, precision);
    value = floor(value * scale) / scale;
    return (value);
}

inline double SetRound(double value, const int precision) {
    double scale = pow(10.0, precision);
    value = rint(value * scale) / scale;
    return (value);
}

inline double SetCeil(double value, const int precision) {
    double scale = pow(10.0, precision);
    value = ceil(value * scale) / scale;
    return (value);
}

inline double Scale (const double pixres, const double polarRadius, 
                     const double equiRadius, const double trueLat = 0.0) {
  double lat = trueLat * Isis::PI / 180.0;
  double a = polarRadius * cos(lat);
  double b = equiRadius * sin(lat);
  double localRadius = equiRadius * polarRadius / sqrt(a*a + b*b);
  return (localRadius / pixres * pi_c() / 180.0);
}

void IsisMain() {
  Process p;

  // Get the list of names of input CCD cubes to stitch together
  FileList flist;
  UserInterface &ui = Application::GetUserInterface();
  flist.Read(ui.GetFilename("FROMLIST"));
  if (flist.size() < 1) {
    string msg = "The list file[" + ui.GetFilename("FROMLIST") +
    " does not contain any filenames";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  string projection("Equirectangular");
  if(ui.WasEntered("MAP")) {
      Pvl mapfile(ui.GetFilename("MAP"));
      projection = (string) mapfile.FindGroup("Mapping")["ProjectionName"];
  }

  if(ui.WasEntered("PROJECTION")) {
      projection = ui.GetString("PROJECTION");
  }

  // Gather other user inputs to projection
  string lattype = ui.GetString("LATTYPE");
  string londir  = ui.GetString("LONDIR");
  string londom  = ui.GetString("LONDOM");
  int digits = ui.GetInteger("PRECISION");

  // Fix them for mapping group
  lattype = (lattype == "PLANETOCENTRIC") ? "Planetocentric" : "Planetographic";
  londir = (londir == "POSITIVEEAST") ? "PositiveEast" : "PositiveWest";

  Progress prog;
  prog.SetMaximumSteps(flist.size());
  prog.CheckStatus();

  Statistics scaleStat;
  Statistics longitudeStat;
  Statistics latitudeStat;
  Statistics equiRadStat;
  Statistics poleRadStat;
  PvlObject fileset("FileSet");

  // Save major equitorial and polar radii for last occuring
  double eqRad; 
  double eq2Rad; 
  double poleRad;

  string target("Unknown");
  for (unsigned int i = 0 ; i < flist.size() ; i++) {
    // Set the input image, get the camera model, and a basic mapping
    // group
    Cube cube;
    cube.Open(flist[i]);

    int lines = cube.Lines();
    int samples = cube.Samples();


    PvlObject fmap("File");
    fmap += PvlKeyword("Name",flist[i]);
    fmap += PvlKeyword("Lines", lines);
    fmap += PvlKeyword("Samples", samples);

    Camera *cam = cube.Camera();
    Pvl mapping;
    cam->BasicMapping(mapping);
    PvlGroup &mapgrp = mapping.FindGroup("Mapping");
    mapgrp.AddKeyword(PvlKeyword("ProjectionName",projection),Pvl::Replace);    
    mapgrp.AddKeyword(PvlKeyword("LatitudeType",lattype),Pvl::Replace);    
    mapgrp.AddKeyword(PvlKeyword("LongitudeDirection",londir),Pvl::Replace);    
    mapgrp.AddKeyword(PvlKeyword("LongitudeDomain",londom),Pvl::Replace);    

    // Get the radii
    double radii[3];
    cam->Radii(radii);

    eqRad   = radii[0] * 1000.0;
    eq2Rad  = radii[1] * 1000.0;
    poleRad = radii[2] * 1000.0;

    target = cam->Target();
    equiRadStat.AddData(&eqRad, 1);
    poleRadStat.AddData(&poleRad, 1);

    // Get resolution
    double lowres = cam->LowestImageResolution();
    double hires = cam->HighestImageResolution();
    scaleStat.AddData(&lowres, 1);
    scaleStat.AddData(&hires, 1);

    double pixres = (lowres+hires)/2.0;
    double scale = Scale(pixres, poleRad, eqRad);
    mapgrp.AddKeyword(PvlKeyword("PixelResolution",pixres),Pvl::Replace);
    mapgrp.AddKeyword(PvlKeyword("Scale",scale,"pixels/degree"),Pvl::Replace);
    mapgrp += PvlKeyword("MinPixelResolution",lowres,"meters");
    mapgrp += PvlKeyword("MaxPixelResolution",hires,"meters");

    // Get the universal ground range
    double minlat,maxlat,minlon,maxlon;
    cam->GroundRange(minlat,maxlat,minlon,maxlon,mapping);
    mapgrp.AddKeyword(PvlKeyword("MinimumLatitude",minlat),Pvl::Replace);
    mapgrp.AddKeyword(PvlKeyword("MaximumLatitude",maxlat),Pvl::Replace);
    mapgrp.AddKeyword(PvlKeyword("MinimumLongitude",minlon),Pvl::Replace);
    mapgrp.AddKeyword(PvlKeyword("MaximumLongitude",maxlon),Pvl::Replace);

    fmap.AddGroup(mapgrp);
    fileset.AddObject(fmap);

    longitudeStat.AddData(&minlon, 1);
    longitudeStat.AddData(&maxlon, 1);
    latitudeStat.AddData(&minlat, 1);
    latitudeStat.AddData(&maxlat, 1);

    p.ClearInputCubes();
    prog.CheckStatus();
  }

//  Construct the output mapping group with statistics
  PvlGroup mapping("Mapping");
  double avgPixRes((scaleStat.Minimum()+scaleStat.Maximum())/2.0);
  double avgLat((latitudeStat.Minimum()+latitudeStat.Maximum())/2.0);
  double avgLon((longitudeStat.Minimum()+longitudeStat.Maximum())/2.0);
  double avgEqRad((equiRadStat.Minimum()+equiRadStat.Maximum())/2.0);
  double avgPoleRad((poleRadStat.Minimum()+poleRadStat.Maximum())/2.0);
  double scale  = Scale(avgPixRes, avgPoleRad, avgEqRad);

  mapping += PvlKeyword("ProjectionName",projection);
  mapping += PvlKeyword("TargetName", target);
  mapping += PvlKeyword("EquatorialRadius",eqRad,"meters");
  mapping += PvlKeyword("PolarRadius",poleRad,"meters");
  mapping += PvlKeyword("LatitudeType",lattype);
  mapping += PvlKeyword("LongitudeDirection",londir);
  mapping += PvlKeyword("LongitudeDomain",londom);
  mapping += PvlKeyword("PixelResolution", SetRound(avgPixRes, digits), "meters/pixel");
  mapping += PvlKeyword("Scale", SetRound(scale, digits), "pixels/degree");
  mapping += PvlKeyword("MinPixelResolution",scaleStat.Minimum(),"meters");
  mapping += PvlKeyword("MaxPixelResolution",scaleStat.Maximum(),"meters");
  mapping += PvlKeyword("CenterLongitude", SetRound(avgLon,digits));
  mapping += PvlKeyword("CenterLatitude",  SetRound(avgLat,digits));
  mapping += PvlKeyword("MinimumLatitude", MAX(SetFloor(latitudeStat.Minimum(),digits), -90.0));
  mapping += PvlKeyword("MaximumLatitude", MIN(SetCeil(latitudeStat.Maximum(),digits), 90.0));
  mapping += PvlKeyword("MinimumLongitude",MAX(SetFloor(longitudeStat.Minimum(),digits), -180.0));
  mapping += PvlKeyword("MaximumLongitude",MIN(SetCeil(longitudeStat.Maximum(),digits), 360.0));

  PvlKeyword clat("PreciseCenterLongitude", avgLon);
  clat.AddComment("Actual Parameters without precision applied");
  mapping += clat;
  mapping += PvlKeyword("PreciseCenterLatitude",  avgLat);
  mapping += PvlKeyword("PreciseMinimumLatitude", latitudeStat.Minimum());
  mapping += PvlKeyword("PreciseMaximumLatitude", latitudeStat.Maximum());
  mapping += PvlKeyword("PreciseMinimumLongitude",longitudeStat.Minimum());
  mapping += PvlKeyword("PreciseMaximumLongitude",longitudeStat.Maximum());

  
  Application::GuiLog(mapping);

  // Write the output file if requested
  if (ui.WasEntered("TO")) {
    Pvl temp;
    temp.AddGroup(mapping);
    temp.Write(ui.GetFilename("TO","map"));
  }

  if (ui.WasEntered("LOG")) {
    Pvl temp;
    temp.AddObject(fileset);
    temp.Write(ui.GetFilename("LOG","log"));
  }

  p.EndProcess();
}
