#define GUIHELPERS

#include "Isis.h"
#include "Application.h"
#include "Camera.h"
#include "Cube.h"
#include "IException.h"
#include "Pvl.h"
#include "ringscam2map.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void PrintMap();
void LoadMapRes();
void LoadCameraRes();
void LoadMapRange();
void LoadCameraRange();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  helper ["LoadMapRes"] = (void *) LoadMapRes;
  helper ["LoadCameraRes"] = (void *) LoadCameraRes;
  helper ["LoadMapRange"] = (void *) LoadMapRange;
  helper ["LoadCameraRange"] = (void *) LoadCameraRange;
  return helper;
}

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ringscam2map(ui);
}

// Helper function to print out mapfile to session log
void PrintMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));

  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  //Write map file out to the log
  Application::GuiLog(userGrp);
}

// Helper function to get mapping resolution.
void LoadMapRes() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  // Set resolution
  if(userGrp.hasKeyword("Scale")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION", userGrp["Scale"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES", "PPD");
  }
  else if(userGrp.hasKeyword("PixelResolution")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION", userGrp["PixelResolution"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES", "MPP");
  }
  else {
    QString msg = "No resolution value found in [" + ui.GetFileName("MAP") + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}

//Helper function to get camera resolution.
void LoadCameraRes() {
  UserInterface &ui = Application::GetUserInterface();
  QString file = ui.GetCubeName("FROM");

  // Open the input cube, get the camera object, and the cam map projection
  Cube c;
  c.open(file);
  Camera *cam = c.camera();
  Pvl camMap;
  cam->BasicMapping(camMap);
  PvlGroup &camGrp = camMap.findGroup("Mapping");

  ui.Clear("RESOLUTION");
  ui.PutDouble("RESOLUTION", camGrp["PixelResolution"]);

  ui.Clear("PIXRES");
  ui.PutAsString("PIXRES", "MPP");
}

//Helper function to get ground range from map file.
void LoadMapRange() {
  UserInterface &ui = Application::GetUserInterface();

  // Get map file
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  // Set ground range keywords that are found in mapfile
  int count = 0;
  ui.Clear("MINRINGRAD");
  ui.Clear("MAXRINGRAD");
  ui.Clear("MINRINGLON");
  ui.Clear("MAXRINGLON");
  if(userGrp.hasKeyword("MinimumRingRadius")) {
    ui.PutDouble("MINRINGRAD", userGrp["MinimumRingRadius"]);
    count++;
  }
  if(userGrp.hasKeyword("MaximumRingRadius")) {
    ui.PutDouble("MAXRINGRAD", userGrp["MaximumRingRadius"]);
    count++;
  }
  if(userGrp.hasKeyword("MinimumRingLongitude")) {
    ui.PutDouble("MINRINGLON", userGrp["MinimumRingLongitude"]);
    count++;
  }
  if(userGrp.hasKeyword("MaximumRingLongitude")) {
    ui.PutDouble("MAXRINGLON", userGrp["MaximumRingLongitude"]);
    count++;
  }

  // Set default ground range param to map
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE", "MAP");

  if(count < 4) {
    QString msg = "One or more of the values for the ground range was not found";
    msg += " in [" + ui.GetFileName("MAP") + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}

//Helper function to load camera range.
void LoadCameraRange() {
  UserInterface &ui = Application::GetUserInterface();
  QString file = ui.GetCubeName("FROM");

  // Get the map projection file provided by the user
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));

  // Open the input cube, get the camera object, and the cam map projection
  Cube c;
  c.open(file);
  Camera *cam = c.camera();

  // Make the target info match the user mapfile
  double minrad, maxrad, minaz, maxaz;
  cam->ringRange(minrad, maxrad, minaz, maxaz, userMap);

  // Set ground range parameters in UI
  ui.Clear("MINRINGRAD");
  ui.PutDouble("MINRINGRAD", minrad);
  ui.Clear("MAXRINGRAD");
  ui.PutDouble("MAXRINGRAD", maxrad);
  ui.Clear("MINRINGLON");
  ui.PutDouble("MINRINGLON", minaz);
  ui.Clear("MAXRINGLON");
  ui.PutDouble("MAXRINGLON", maxaz);

  // Set default ground range param to camera
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE", "CAMERA");
}
