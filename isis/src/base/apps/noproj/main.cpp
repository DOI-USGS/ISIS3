#define GUIHELPERS
#include "Isis.h"

#include "noproj.h"

#include "Application.h"
#include "CameraDetectorMap.h"

using namespace std;
using namespace Isis;

static void LoadMatchSummingMode();
static void LoadInputSummingMode();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["LoadMatchSummingMode"] = (void *) LoadMatchSummingMode;
  helper ["LoadInputSummingMode"] = (void *) LoadInputSummingMode;
  return helper;
}

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  noproj(ui);
}

// Helper function to get output summing mode from cube to MATCH
void LoadMatchSummingMode() {
  QString file;
  UserInterface &ui = Application::GetUserInterface();

  // Get camera from cube to match
  if((ui.GetString("SOURCE") == "FROMMATCH") && (ui.WasEntered("MATCH"))) {
    file = ui.GetCubeName("MATCH");
  }
  else {
    file = ui.GetCubeName("FROM");
  }

// Open the input cube and get the camera object
  Cube c;
  c.open(file);
  Camera *cam = c.camera();

  ui.Clear("SUMMINGMODE");
  ui.PutDouble("SUMMINGMODE", cam->DetectorMap()->SampleScaleFactor());

  ui.Clear("SOURCE");
  ui.PutAsString("SOURCE", "FROMUSER");
}

// Helper function to get output summing mode from input cube (FROM)
void LoadInputSummingMode() {
  UserInterface &ui = Application::GetUserInterface();

  // Get camera from cube to match
  QString file = ui.GetCubeName("FROM");
  // Open the input cube and get the camera object
  Cube c;
  c.open(file);
  Camera *cam = c.camera();

  ui.Clear("SUMMINGMODE");
  ui.PutDouble("SUMMINGMODE", cam->DetectorMap()->SampleScaleFactor());

  ui.Clear("SOURCE");
  ui.PutAsString("SOURCE", "FROMUSER");
}
