#include "Isis.h"

#include "Camera.h"
#include "Process.h"
#include "Projection.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Set the input image, get the camera model
  Process p;
  Cube *icube = p.SetInputCube("FROM");
  Camera *cam = icube->camera();

  // Get the ra/dec range and resolution
  double minRa, maxRa, minDec, maxDec;
  cam->RaDecRange(minRa, maxRa, minDec, maxDec);
  double res = cam->RaDecResolution();

  // Get the center ra/dec
  cam->SetImage(icube->sampleCount() / 2.0, icube->lineCount() / 2.0);
  double centerRa  = cam->RightAscension();
  double centerDec = cam->Declination();

  // Compute the rotation
  cam->SetRightAscensionDeclination(centerRa, centerDec + 2.0 * res);
  double x = cam->Sample() - icube->sampleCount() / 2.0;
  double y = cam->Line() - icube->lineCount() / 2.0;
  double rot = atan2(-y, x) * 180.0 / Isis::PI;
  rot = 90.0 - rot;
  if(rot < 0.0) rot += 360.0;

  // Setup and log results
  PvlGroup results("Range");
  results += PvlKeyword("MinimumRightAscension", toString(minRa), "degrees");
  results += PvlKeyword("MaximumRightAscension", toString(maxRa), "degrees");
  results += PvlKeyword("MinimumDeclination", toString(minDec), "degrees");
  results += PvlKeyword("MaximumDeclination", toString(maxDec), "degrees");
  results += PvlKeyword("MinimumRightAscension", Projection::ToHMS(minRa), "hms");
  results += PvlKeyword("MaximumRightAscension", Projection::ToHMS(maxRa), "hms");
  results += PvlKeyword("MinimumDeclination", Projection::ToDMS(minDec), "dms");
  results += PvlKeyword("MaximumDeclination", Projection::ToDMS(maxDec), "dms");
  results += PvlKeyword("Resolution", toString(res), "degrees/pixel");
  Application::Log(results);

  // Setup and log orientation
  PvlGroup orient("Orientation");
  orient += PvlKeyword("CenterSample", toString(icube->sampleCount() / 2.0));
  orient += PvlKeyword("CenterLine", toString(icube->lineCount() / 2.0));
  orient += PvlKeyword("CenterRightAscension", toString(centerRa), "degrees");
  orient += PvlKeyword("CenterDeclination", toString(centerDec), "degrees");
  orient += PvlKeyword("CelestialNorthClockAngle", toString(rot), "degrees");
  orient += PvlKeyword("Resolution", toString(res), "degrees/pixel");
  Application::Log(orient);

  // Write the output file if requested
  UserInterface ui = Application::GetUserInterface();
  if(ui.WasEntered("TO")) {
    Pvl temp;
    temp.addGroup(results);
    temp.addGroup(orient);
    temp.write(ui.GetFileName("TO", "txt"));
  }

  p.EndProcess();
}
