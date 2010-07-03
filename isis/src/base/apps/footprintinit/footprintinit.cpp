#include "Isis.h"

#include "iException.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"
#include "Process.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "SerialNumber.h"


using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Cube cube;
  cube.Open(ui.GetFilename("FROM"), "rw");

  // Make sure cube has been run through spiceinit
  try {
    cube.Camera();
  }
  catch(iException &e) {
    string msg = "Spiceinit must be run before initializing the polygon";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  Progress prog;
  prog.SetMaximumSteps(1);
  prog.CheckStatus();

  std::string sn = SerialNumber::Compose(cube);

  ImagePolygon poly;
  if(ui.WasEntered("MAXEMISSION")) {
    poly.Emission(ui.GetDouble("MAXEMISSION"));
  }
  if(ui.WasEntered("MAXINCIDENCE")) {
    poly.Incidence(ui.GetDouble("MAXINCIDENCE"));
  }
  if(ui.GetString("LIMBTEST") == "ELLIPSOID") {
    poly.EllipsoidLimb(true);
  }

  // Reduce the increment size to find a valid polygon
  int sinc = ui.GetInteger("SINC");
  int linc = ui.GetInteger("LINC");
  bool precision = ui.GetBoolean("INCREASEPRECISION");
  while(true) {
    try {
      poly.Create(cube, sinc, linc);
      break;
    }
    catch(iException &e) {
      if(precision && sinc > 1 && linc > 1) {
        sinc = sinc * 2 / 3;
        linc = linc * 2 / 3;
        e.Clear();
      }
      else {
        e.Report();
        return; // Prevents infinite loops caused by **SPICE ERROR**
      }
    }
  }


  if(ui.GetBoolean("TESTXY")) {
    Pvl cubeLab(ui.GetFilename("FROM"));
    PvlGroup inst = cubeLab.FindGroup("Instrument", Pvl::Traverse);
    string target = inst["TargetName"];
    PvlGroup radii = Projection::TargetRadii(target);

    Pvl map(ui.GetFilename("MAP"));
    PvlGroup &mapping = map.FindGroup("MAPPING");

    if(!mapping.HasKeyword("TargetName"))
      mapping += Isis::PvlKeyword("TargetName", target);
    if(!mapping.HasKeyword("EquatorialRadius"))
      mapping += Isis::PvlKeyword("EquatorialRadius", (string)radii["EquatorialRadius"]);
    if(!mapping.HasKeyword("PolarRadius"))
      mapping += Isis::PvlKeyword("PolarRadius", (string)radii["PolarRadius"]);
    if(!mapping.HasKeyword("LatitudeType"))
      mapping += Isis::PvlKeyword("LatitudeType", "Planetocentric");
    if(!mapping.HasKeyword("LongitudeDirection"))
      mapping += Isis::PvlKeyword("LongitudeDirection", "PositiveEast");
    if(!mapping.HasKeyword("LongitudeDomain"))
      mapping += Isis::PvlKeyword("LongitudeDomain", 360);
    if(!mapping.HasKeyword("CenterLatitude"))
      mapping += Isis::PvlKeyword("CenterLatitude", 0);
    if(!mapping.HasKeyword("CenterLongitude"))
      mapping += Isis::PvlKeyword("CenterLongitude", 0);

    while(true) {
      try {
        Projection *proj = ProjectionFactory::Create(map, true);
        geos::geom::MultiPolygon *xyPoly = PolygonTools::LatLonToXY(*poly.Polys(), proj);

        delete proj;
        proj = NULL;
        delete xyPoly;
        xyPoly = NULL;

        break;
      }
      catch(iException &e) {
        if(precision && sinc > 1 && linc > 1) {
          sinc = sinc * 2 / 3;
          linc = linc * 2 / 3;
          poly.Create(cube, sinc, linc);
          e.Clear();
        }
        else {
          e.Report();
          return; // Prevents infinite loops caused by **SPICE ERROR**
        }
      }
    }

  }



  cube.BlobDelete("Polygon", sn);
  cube.Write(poly);

  if(precision) {
    PvlGroup results("Results");
    results.AddKeyword(PvlKeyword("SINC", sinc));
    results.AddKeyword(PvlKeyword("LINC", linc));
    Application::Log(results);
  }

  Process p;
  p.SetInputCube("FROM");
  p.WriteHistory(cube);

  cube.Close();
  prog.CheckStatus();
}
