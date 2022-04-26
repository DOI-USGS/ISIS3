#define GUIHELPERS
#include "Isis.h"

#include "Application.h"
#include "map2map.h"

using namespace Isis;
using namespace std;

void PrintMap();
void LoadMapRange();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  helper ["LoadMapRange"] = (void *) LoadMapRange;
  return helper;
}

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    map2map(ui, &appLog);
  }
  catch (...) {
    for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
      Application::Log(*grpIt);
    }
    throw;
  }

  for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
    Application::Log(*grpIt);
  }
}

// Helper function to print out mapfile to session log
void PrintMap() {
UserInterface &ui = Application::GetUserInterface();

// Get mapping group from map file
Pvl userMap;
userMap.read(ui.GetFileName("MAP"));
PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

//Write map file out to the log
Isis::Application::GuiLog(userGrp);
}

void LoadMapRange() {
UserInterface &ui = Application::GetUserInterface();

// Get map file
Pvl userMap;

try {
userMap.read(ui.GetFileName("MAP"));
}
catch(IException &e) {
}

// Get input cube
Pvl fromMap;

try {
fromMap.read(ui.GetCubeName("FROM"));
}
catch(IException &e) {
}

// Try to get the mapping groups
PvlGroup fromMapping("Mapping");

try {
fromMapping = fromMap.findGroup("Mapping", Pvl::Traverse);
}
catch(IException &e) {
}

PvlGroup userMapping("Mapping");

try {
userMapping = userMap.findGroup("Mapping", Pvl::Traverse);
}
catch(IException &e) {
}

// Do conversions on from map

// Longitude conversions first
if(userMapping.hasKeyword("LongitudeDirection")) {
if(((QString)userMapping["LongitudeDirection"]).compare(fromMapping["LongitudeDirection"]) != 0) {
  double minLon = fromMapping["MinimumLongitude"];
  double maxLon = fromMapping["MaximumLongitude"];
  int domain = fromMapping["LongitudeDomain"];

  if(userMapping.hasKeyword("LongitudeDomain")) {
    domain = userMapping["LongitudeDomain"];
  }

  if((QString)userMapping["LongitudeDirection"] == "PositiveEast") {
    fromMapping["MaximumLongitude"] = toString(TProjection::ToPositiveEast(minLon, domain));
    fromMapping["MinimumLongitude"] = toString(TProjection::ToPositiveEast(maxLon, domain));
  }
  else if((QString)userMapping["LongitudeDirection"] == "PositiveWest") {
    fromMapping["MaximumLongitude"] = toString(TProjection::ToPositiveWest(minLon, domain));
    fromMapping["MinimumLongitude"] = toString(TProjection::ToPositiveWest(maxLon, domain));
  }
}
}

// Latitude conversions now
if(userMapping.hasKeyword("LatitudeType")) { // user set a new domain?
if(((QString)userMapping["LatitudeType"]).compare(fromMapping["LatitudeType"]) != 0) { // new lat type different?
  if(((QString)userMapping["LatitudeType"]).compare("Planetographic") == 0) {
    fromMapping["MinimumLatitude"] = toString(TProjection::ToPlanetographic(
                                       (double)fromMapping["MinimumLatitude"],
                                       (double)fromMapping["EquatorialRadius"],
                                       (double)fromMapping["PolarRadius"]));
    fromMapping["MaximumLatitude"] = toString(TProjection::ToPlanetographic(
                                       (double)fromMapping["MaximumLatitude"],
                                       (double)fromMapping["EquatorialRadius"],
                                       (double)fromMapping["PolarRadius"]));
  }
  else {
    fromMapping["MinimumLatitude"] = toString(TProjection::ToPlanetocentric(
                                       (double)fromMapping["MinimumLatitude"],
                                       (double)fromMapping["EquatorialRadius"],
                                       (double)fromMapping["PolarRadius"]));
    fromMapping["MaximumLatitude"] = toString(TProjection::ToPlanetocentric(
                                       (double)fromMapping["MaximumLatitude"],
                                       (double)fromMapping["EquatorialRadius"],
                                       (double)fromMapping["PolarRadius"]));
  }
}
}

// Failed at longitudes, use our originals!
if((double)fromMapping["MinimumLongitude"] >= (double)fromMapping["MaximumLongitude"]) {
try {
  fromMapping["MinimumLongitude"] = fromMap.findGroup("Mapping", Pvl::Traverse)["MinimumLongitude"];
  fromMapping["MaximumLongitude"] = fromMap.findGroup("Mapping", Pvl::Traverse)["MaximumLongitude"];
}
catch(IException &e) {
}
}

// Overlay lat/lons in map file (if DEFAULTRANGE=MAP)
if(ui.GetString("DEFAULTRANGE") == "MAP") {
if(userMapping.hasKeyword("MinimumLatitude")) {
  fromMapping["MinimumLatitude"] = userMapping["MinimumLatitude"];
}

if(userMapping.hasKeyword("MaximumLatitude")) {
  fromMapping["MaximumLatitude"] = userMapping["MaximumLatitude"];
}

if(userMapping.hasKeyword("MinimumLongitude")) {
  fromMapping["MinimumLongitude"] = userMapping["MinimumLongitude"];
}

if(userMapping.hasKeyword("MaximumLongitude")) {
  fromMapping["MaximumLongitude"] = userMapping["MaximumLongitude"];
}
}

if(ui.WasEntered("MINLAT")) {
ui.Clear("MINLAT");
}

if(ui.WasEntered("MAXLAT")) {
ui.Clear("MAXLAT");
}

if(ui.WasEntered("MINLON")) {
ui.Clear("MINLON");
}

if(ui.WasEntered("MAXLON")) {
ui.Clear("MAXLON");
}

ui.PutDouble("MINLAT", fromMapping["MinimumLatitude"]);
ui.PutDouble("MAXLAT", fromMapping["MaximumLatitude"]);
ui.PutDouble("MINLON", fromMapping["MinimumLongitude"]);
ui.PutDouble("MAXLON", fromMapping["MaximumLongitude"]);
}
