#include "Isis.h"
#include "Process.h"
#include "Camera.h"
#include "Pvl.h"

using namespace std; 
using namespace Isis;

void IsisMain() {
  Process p;
  
  // Set the input image, get the camera model, and a basic mapping
  // group
  Cube *icube = p.SetInputCube("FROM");
  Camera *cam = icube->Camera();
  Pvl mapping;
  cam->BasicMapping(mapping);
  PvlGroup &mapgrp = mapping.FindGroup("Mapping");
  
  // Setup the output results by first adding the filename
  UserInterface &ui = Application::GetUserInterface();

  // Get the radii
  double radii[3];
  cam->Radii(radii);
  PvlGroup target("Target");
  target += PvlKeyword("From",ui.GetFilename("FROM"));
  target += PvlKeyword("TargetName",cam->Target());
  target += PvlKeyword("RadiusA",radii[0]*1000.0,"meters");
  target += PvlKeyword("RadiusB",radii[1]*1000.0,"meters");
  target += PvlKeyword("RadiusC",radii[2]*1000.0,"meters");

  // Get resolution
  PvlGroup res("PixelResolution");
  double lowres = cam->LowestImageResolution();
  double hires = cam->HighestImageResolution();
  res += PvlKeyword("Lowest",lowres,"meters");
  res += PvlKeyword("Highest",hires,"meters");

  // Get the universal ground range
  PvlGroup ugr("UniversalGroundRange");
  double minlat,maxlat,minlon,maxlon;
  cam->GroundRange(minlat,maxlat,minlon,maxlon,mapping);
  ugr += PvlKeyword("LatitudeType","Planetocentric");
  ugr += PvlKeyword("LongitudeDirection","PositiveEast");
  ugr += PvlKeyword("LongitudeDomain",360);
  ugr += PvlKeyword("MinimumLatitude",minlat);
  ugr += PvlKeyword("MaximumLatitude",maxlat);
  ugr += PvlKeyword("MinimumLongitude",minlon);
  ugr += PvlKeyword("MaximumLongitude",maxlon);

  // Get the ographic latitude range
  mapgrp.AddKeyword(PvlKeyword("LatitudeType","Planetographic"),
                    Pvl::Replace);
  cam->GroundRange(minlat,maxlat,minlon,maxlon,mapping);
  PvlGroup ogr("LatitudeRange");
  ogr += PvlKeyword("LatitudeType","Planetographic");
  ogr += PvlKeyword("MinimumLatitude",minlat);
  ogr += PvlKeyword("MaximumLatitude",maxlat);

  // Get positive west longitude coordinates in 360 domain
  mapgrp.AddKeyword(PvlKeyword("LongitudeDirection","PositiveWest"),
                    Pvl::Replace);
  cam->GroundRange(minlat,maxlat,minlon,maxlon,mapping);
  PvlGroup pos360("PositiveWest360");
  pos360 += PvlKeyword("LongitudeDirection","PositiveWest");
  pos360 += PvlKeyword("LongitudeDomain",360);
  pos360 += PvlKeyword("MinimumLongitude",minlon);
  pos360 += PvlKeyword("MaximumLongitude",maxlon);

  // Get positive east longitude coordinates in 180 domain
  mapgrp.AddKeyword(PvlKeyword("LongitudeDirection","PositiveEast"),
                    Pvl::Replace);
  mapgrp.AddKeyword(PvlKeyword("LongitudeDomain","180"),
                    Pvl::Replace);
  cam->GroundRange(minlat,maxlat,minlon,maxlon,mapping);
  PvlGroup pos180("PositiveEast180");
  pos180 += PvlKeyword("LongitudeDirection","PositiveEast");
  pos180 += PvlKeyword("LongitudeDomain",180);
  pos180 += PvlKeyword("MinimumLongitude",minlon);
  pos180 += PvlKeyword("MaximumLongitude",maxlon);

  // Get positive west longitude coordinates in 180 domain
  mapgrp.AddKeyword(PvlKeyword("LongitudeDirection","PositiveWest"),
                    Pvl::Replace);
  cam->GroundRange(minlat,maxlat,minlon,maxlon,mapping);
  PvlGroup neg180("PositiveWest180");
  neg180 += PvlKeyword("LongitudeDirection","PositiveWest");
  neg180 += PvlKeyword("LongitudeDomain",180);
  neg180 += PvlKeyword("MinimumLongitude",minlon);
  neg180 += PvlKeyword("MaximumLongitude",maxlon);

  // Write it to the log
  Application::Log(target);
  Application::Log(res);
  Application::Log(ugr);
  Application::Log(ogr);
  Application::Log(pos360);
  Application::Log(pos180);
  Application::Log(neg180);
  
  // Write the output file if requested
  if (ui.WasEntered("TO")) {
    Pvl temp;
    temp.AddGroup(target);
    temp.AddGroup(res);
    temp.AddGroup(ugr);
    temp.AddGroup(ogr);
    temp.AddGroup(pos360);
    temp.AddGroup(pos180);
    temp.AddGroup(neg180);
    temp.Write(ui.GetFilename("TO","txt"));
  }

  p.EndProcess();
}
