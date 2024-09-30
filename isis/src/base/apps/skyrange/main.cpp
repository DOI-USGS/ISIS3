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
  double rot = cam->CelestialNorthClockAngle();

  // Setup and log results
  PvlGroup results("Range");
  results += PvlKeyword("MinimumRightAscension", Isis::toString(minRa), "degrees");
  results += PvlKeyword("MaximumRightAscension", Isis::toString(maxRa), "degrees");
  results += PvlKeyword("MinimumDeclination", Isis::toString(minDec), "degrees");
  results += PvlKeyword("MaximumDeclination", Isis::toString(maxDec), "degrees");
  results += PvlKeyword("MinimumRightAscension", Projection::ToHMS(minRa).toStdString(), "hms");
  results += PvlKeyword("MaximumRightAscension", Projection::ToHMS(maxRa).toStdString(), "hms");
  results += PvlKeyword("MinimumDeclination", Projection::ToDMS(minDec).toStdString(), "dms");
  results += PvlKeyword("MaximumDeclination", Projection::ToDMS(maxDec).toStdString(), "dms");
  results += PvlKeyword("Resolution", Isis::toString(res), "degrees/pixel");
  Application::Log(results);

  // Setup and log orientation
  PvlGroup orient("Orientation");
  orient += PvlKeyword("CenterSample", Isis::toString(icube->sampleCount() / 2.0));
  orient += PvlKeyword("CenterLine", Isis::toString(icube->lineCount() / 2.0));
  orient += PvlKeyword("CenterRightAscension", Isis::toString(centerRa), "degrees");
  orient += PvlKeyword("CenterDeclination", Isis::toString(centerDec), "degrees");
  orient += PvlKeyword("CelestialNorthClockAngle", Isis::toString(rot), "degrees");
  orient += PvlKeyword("Resolution", Isis::toString(res), "degrees/pixel");
  Application::Log(orient);

  // Write the output file if requested
  UserInterface ui = Application::GetUserInterface();
  if(ui.WasEntered("TO")) {
    Pvl temp;
    temp.addGroup(results);
    temp.addGroup(orient);
    temp.write(ui.GetFileName("TO", "txt").toStdString());
  }

  p.EndProcess();
}
