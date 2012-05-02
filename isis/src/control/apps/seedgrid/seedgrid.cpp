#define GUIHELPERS

#include "Isis.h"

#include "ControlNet.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "ID.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Progress.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "SurfacePoint.h"

using namespace std;
using namespace Isis;

void checkLatitude(double minLat, double maxLat);
void checkLongitude(double minLon, double maxLon, int lonDomain);
void printMap();

map<string, void *> GuiHelpers() {
  map<string, void *> helper;
  helper["PrintMap"] = (void *) printMap;
  return helper;
}

void IsisMain() {

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();

  ControlNet cnet;

  double equatorialRadius = 0.0;

  string spacing = ui.GetString("SPACING");
  if (spacing == "METER") {
    Pvl userMap;
    userMap.Read(ui.GetFileName("MAP"));
    PvlGroup &mapGroup = userMap.FindGroup("Mapping", Pvl::Traverse);

    // Construct a Projection for converting between Lon/Lat and X/Y
    // TODO: Should this be an option to include this in the program?
    string target;
    if (ui.WasEntered("TARGET")) {
      target = ui.GetString("TARGET");
    }
    else if (mapGroup.HasKeyword("TargetName")) {
      target = mapGroup.FindKeyword("TargetName")[0];
      ui.PutAsString("TARGET", target);
    }
    else {
      string msg = "A target must be specified either by the [TARGET] "
          "parameter or included as a value for keyword [TargetName] in the "
          "projection file";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    mapGroup.AddKeyword(PvlKeyword("TargetName", target), Pvl::Replace);

    if (!mapGroup.HasKeyword("EquatorialRadius") ||
        !mapGroup.HasKeyword("PolarRadius")) {

      PvlGroup radii = Projection::TargetRadii(target);
      mapGroup.AddKeyword(PvlKeyword("EquatorialRadius",
            (string) radii["EquatorialRadius"]));
      mapGroup.AddKeyword(PvlKeyword("PolarRadius",
            (string) radii["PolarRadius"]));
    }

    if (!ui.WasEntered("MAP")) {
      mapGroup.AddKeyword(PvlKeyword("LatitudeType", "Planetocentric"));
      mapGroup.AddKeyword(PvlKeyword("LongitudeDirection", "PositiveEast"));
      mapGroup.AddKeyword(PvlKeyword("LongitudeDomain", 360));
      mapGroup.AddKeyword(PvlKeyword("CenterLatitude", 0));
      mapGroup.AddKeyword(PvlKeyword("CenterLongitude", 0));
    }

    double minLat = ui.GetDouble("MINLAT");
    double maxLat = ui.GetDouble("MAXLAT");
    double minLon = ui.GetDouble("MINLON");
    double maxLon = ui.GetDouble("MAXLON");
    checkLatitude(minLat, maxLat);

    mapGroup.AddKeyword(PvlKeyword("MinimumLatitude", minLat), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MaximumLatitude", maxLat), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MinimumLongitude", minLon), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MaximumLongitude", maxLon), Pvl::Replace);

    Projection *proj = ProjectionFactory::Create(userMap);

    int lonDomain = (int) iString(proj->LongitudeDomainString());
    checkLongitude(minLon, maxLon, lonDomain);

    // Convert the Lat/Lon range to an X/Y range.
    double minX;
    double minY;
    double maxX;
    double maxY;
    bool foundRange = proj->XYRange(minX, maxX, minY, maxY);
    if (!foundRange) {
      string msg = "Cannot convert Lat/Long range to an X/Y range";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Create the control net to store the points in.
    cnet.SetTarget(target);
    string networkId;
    if (ui.WasEntered("NETWORKID")) {
      networkId = ui.GetString("NETWORKID");
      cnet.SetNetworkId(networkId);
    }
    cnet.SetUserName(Isis::Application::UserName());
    if (ui.WasEntered("DESCRIPTION")) {
      cnet.SetDescription(ui.GetString("DESCRIPTION"));
    }

    // Set up an automatic id generator for the point ids
    ID pointId = ID(ui.GetString("POINTID"));

    double xStepSize = ui.GetDouble("XSTEP");
    double yStepSize = ui.GetDouble("YSTEP");

    equatorialRadius = mapGroup.FindKeyword("EquatorialRadius")[0];

    Progress gridStatus;

    int maxSteps = 0;
    double x = minX;
    while (x <= maxX) {
      double y = minY;
      while (y <= maxY) {
        maxSteps++;
        y += yStepSize;
      }
      x += xStepSize;
    }

    if (maxSteps > 0) {
      gridStatus.SetMaximumSteps(maxSteps);
      gridStatus.SetText("Seeding Grid");
      gridStatus.CheckStatus();
    }

    x = minX;
    while (x <= maxX) {
      double y = minY;
      while (y <= maxY) {
        proj->SetCoordinate(x, y);
        if (!proj->IsSky() && proj->Latitude() < ui.GetDouble("MAXLAT") &&
            proj->Longitude() < ui.GetDouble("MAXLON") &&
            proj->Latitude() > ui.GetDouble("MINLAT") &&
            proj->Longitude() > ui.GetDouble("MINLON")) {
          SurfacePoint pt(Latitude(proj->Latitude(), Angle::Degrees),
                          Longitude(proj->Longitude(), Angle::Degrees),
                          Distance(equatorialRadius, Distance::Meters));
          ControlPoint * control = new ControlPoint;
          control->SetId(pointId.Next());
          control->SetIgnored(true);
          control->SetAprioriSurfacePoint(pt);
          cnet.AddPoint(control);
        }
        y += yStepSize;
        gridStatus.CheckStatus();
      }
      x += xStepSize;
    }
  }
  else {

    if (!ui.WasEntered("TARGET")) {
      string msg = "A target must be specified by the [TARGET] parameter ";
      msg += "or included as a value for keyword [TargetName] in the projection file";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    string target = ui.GetString("TARGET");
    PvlGroup radii = Projection::TargetRadii(target);
    equatorialRadius = radii["EquatorialRadius"];

    // Create the control net to store the points in.
    cnet.SetTarget(target);
    string networkId;
    if (ui.WasEntered("NETWORKID")) {
      networkId = ui.GetString("NETWORKID");
      cnet.SetNetworkId(networkId);
    }
    cnet.SetUserName(Isis::Application::UserName());
    if (ui.WasEntered("DESCRIPTION")) {
      cnet.SetDescription(ui.GetString("DESCRIPTION"));
    }

    // Set up an automatic id generator for the point ids
    ID pointId = ID(ui.GetString("POINTID"));

    double minLat = ui.GetDouble("MINLAT");
    double maxLat = ui.GetDouble("MAXLAT");
    double latStep = ui.GetDouble("LATSTEP");
    checkLatitude(minLat, maxLat);

    double minLon = ui.GetDouble("MINLON");
    double maxLon = ui.GetDouble("MAXLON");
    double lonStep = ui.GetDouble("LONSTEP");
    checkLongitude(minLon, maxLon, 360);

    Progress gridStatus;

    int maxSteps = 0;
    double lon = minLon;
    while (lon <= maxLon) {
      double lat = minLat;
      while (lat <= maxLat) {
        maxSteps++;
        lat += latStep;
      }
      lat = minLat;
      lon += lonStep;
    }

    if (maxSteps > 0) {
      gridStatus.SetMaximumSteps(maxSteps);
      gridStatus.SetText("Seeding Grid");
      gridStatus.CheckStatus();
    }

    lon = minLon;
    while (lon <= maxLon) {
      double lat = minLat;
      while (lat <= maxLat) {
        ControlPoint * control = new ControlPoint;
        control->SetId(pointId.Next());
        control->SetIgnored(true);
        SurfacePoint pt(Latitude(lat, Angle::Degrees),
                        Longitude(lon, Angle::Degrees),
                        Distance(equatorialRadius, Distance::Meters));
        control->SetAprioriSurfacePoint(pt);
        cnet.AddPoint(control);

        lat += latStep;
        gridStatus.CheckStatus();
      }
      lon += lonStep;
    }
  }

  PvlGroup results("Results");
  results += PvlKeyword("EquatorialRadius", equatorialRadius);
  results += PvlKeyword("NumberControlPoints", cnet.GetNumPoints());
  Application::Log(results);

  cnet.Write(ui.GetFileName("ONET"));
}


void checkLatitude(double minLat, double maxLat) {
  if (minLat > maxLat) {
    string msg = "MINLAT [" + iString(minLat) +
      "] is greater than MAXLAT [" + iString(maxLat) + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (minLat < -90) {
    string msg = "MINLAT [" + iString(minLat) + "] is less than -90";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (maxLat > 90) {
    string msg = "MAXLAT [" + iString(maxLat) + "] is greater than 90";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}


void checkLongitude(double minLon, double maxLon, int lonDomain) {
  if (minLon > maxLon) {
    double suggestedMaxLon = maxLon + lonDomain + (lonDomain - 360);
    string msg = "MINLON [" + iString(minLon) +
      "] is greater than MAXLON [" + iString(maxLon) + "].  " +
      "If you meant to wrap around the [" + iString(lonDomain) +
      "] longitude " + "boundary, use a MAXLON of [" +
      iString(suggestedMaxLon) + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (minLon < lonDomain - 360) {
    string msg = "MINLON [" + iString(minLon) +
      "] is less than [" + iString(lonDomain) + "] domain minimum [" +
      iString(lonDomain - 360) + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (maxLon - minLon > 360) {
    int range = (int) (maxLon - minLon - 1);
    int loops = range / 360 + 1;
    string msg = "The specified longitude range [" + iString(minLon) +
      "] to [" + iString(maxLon) + "] seeds that same area of the target [" +
      iString(loops) + "] times";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}


// Helper function to print out mapfile to session log
void printMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping", Pvl::Traverse);

  //Write map file out to the log
  Isis::Application::GuiLog(userGrp);
}

