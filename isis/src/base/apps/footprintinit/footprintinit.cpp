#include "Isis.h"

#include "IException.h"
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
  cube.open(ui.GetFileName("FROM"), "rw");

  // Make sure cube has been run through spiceinit
  try {
    cube.getCamera();
  }
  catch(IException &e) {
    string msg = "Spiceinit must be run before initializing the polygon";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  Progress prog;
  prog.SetMaximumSteps(1);
  prog.CheckStatus();

  QString sn = SerialNumber::Compose(cube);

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

  int sinc = 1;
  int linc = 1;
  IString incType = ui.GetString("INCTYPE");
  if(incType.UpCase() == "VERTICES") {
    poly.initCube(cube);
    sinc = linc = (int)(0.5 + (((poly.validSampleDim() * 2) +
                       (poly.validLineDim() * 2) - 3.0) /
                       ui.GetInteger("NUMVERTICES")));
    if (sinc < 1.0 || linc < 1.0)
      sinc = linc = 1.0;
  }
  else if (incType.UpCase() == "LINCSINC"){
    sinc = ui.GetInteger("SINC");
    linc = ui.GetInteger("LINC");
  }
  else {
    string msg = "Invalid INCTYPE option[" + incType + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  bool precision = ui.GetBoolean("INCREASEPRECISION");
  try {
    poly.Create(cube, sinc, linc, 1, 1, 0, 0, 1, precision);
  }
  catch (IException &e) {
    QString msg = "Cannot generate polygon for [" + ui.GetFileName("FROM") + "]";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  if(ui.GetBoolean("TESTXY")) {
    Pvl cubeLab(ui.GetFileName("FROM"));
    PvlGroup inst = cubeLab.FindGroup("Instrument", Pvl::Traverse);
    QString target = inst["TargetName"];
    PvlGroup radii = Projection::TargetRadii(target);

    Pvl map(ui.GetFileName("MAP"));
    PvlGroup &mapping = map.FindGroup("MAPPING");

    if(!mapping.HasKeyword("TargetName"))
      mapping += Isis::PvlKeyword("TargetName", target);
    if(!mapping.HasKeyword("EquatorialRadius"))
      mapping += Isis::PvlKeyword("EquatorialRadius", (QString)radii["EquatorialRadius"]);
    if(!mapping.HasKeyword("PolarRadius"))
      mapping += Isis::PvlKeyword("PolarRadius", (QString)radii["PolarRadius"]);
    if(!mapping.HasKeyword("LatitudeType"))
      mapping += Isis::PvlKeyword("LatitudeType", "Planetocentric");
    if(!mapping.HasKeyword("LongitudeDirection"))
      mapping += Isis::PvlKeyword("LongitudeDirection", "PositiveEast");
    if(!mapping.HasKeyword("LongitudeDomain"))
      mapping += Isis::PvlKeyword("LongitudeDomain", "360");
    if(!mapping.HasKeyword("CenterLatitude"))
      mapping += Isis::PvlKeyword("CenterLatitude", "0");
    if(!mapping.HasKeyword("CenterLongitude"))
      mapping += Isis::PvlKeyword("CenterLongitude", "0");

    sinc = poly.getSinc();
    linc = poly.getLinc();
    bool polygonGenerated = false;
    while (!polygonGenerated) {
      Projection *proj = NULL;
      geos::geom::MultiPolygon *xyPoly = NULL;

      try {
        proj = ProjectionFactory::Create(map, true);
        xyPoly = PolygonTools::LatLonToXY(*poly.Polys(), proj);

        polygonGenerated = true;
      }
      catch (IException &e) {
        if (precision && sinc > 1 && linc > 1) {
          sinc = sinc * 2 / 3;
          linc = linc * 2 / 3;
          poly.Create(cube, sinc, linc);
        }
        else {
          delete proj;
          delete xyPoly;
          e.print(); // This should be a NAIF error
          QString msg = "Cannot calculate XY for [";
          msg += ui.GetFileName("FROM") + "]";
          throw IException(e, IException::User, msg, _FILEINFO_);
        }
      }

      delete proj;
      delete xyPoly;
    }
  }

  cube.deleteBlob("Polygon", sn);
  cube.write(poly);

  if(precision) {
    PvlGroup results("Results");
    results.AddKeyword(PvlKeyword("SINC", toString(sinc)));
    results.AddKeyword(PvlKeyword("LINC", toString(linc)));
    Application::Log(results);
  }

  Process p;
  p.SetInputCube("FROM");
  p.WriteHistory(cube);

  cube.close();
  prog.CheckStatus();
}
