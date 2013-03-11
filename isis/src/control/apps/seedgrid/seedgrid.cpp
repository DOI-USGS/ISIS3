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

map<QString, void *> GuiHelpers() {
  map<QString, void *> helper;
  helper["PrintMap"] = (void *) printMap;
  return helper;
}

void IsisMain() {

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();

  ControlNet cnet;

  double equatorialRadius = 0.0;

  QString spacing = ui.GetString("SPACING");
  if (spacing == "METER") {
    Pvl userMap;
    userMap.read(ui.GetFileName("MAP"));
    PvlGroup &mapGroup = userMap.findGroup("Mapping", Pvl::Traverse);

    // Construct a Projection for converting between Lon/Lat and X/Y
    // TODO: Should this be an option to include this in the program?
    QString target;
    if (ui.WasEntered("TARGET")) {
      target = ui.GetString("TARGET");
    }
    else if (mapGroup.hasKeyword("TargetName")) {
      target = mapGroup.findKeyword("TargetName")[0];
      ui.PutAsString("TARGET", target);
    }
    else {
      QString msg = "A target must be specified either by the [TARGET] "
          "parameter or included as a value for keyword [TargetName] in the "
          "projection file";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    mapGroup.addKeyword(PvlKeyword("TargetName", target), Pvl::Replace);

    if (!mapGroup.hasKeyword("EquatorialRadius") ||
        !mapGroup.hasKeyword("PolarRadius")) {

      PvlGroup radii = Projection::TargetRadii(target);
      mapGroup.addKeyword(PvlKeyword("EquatorialRadius",
            (QString) radii["EquatorialRadius"]));
      mapGroup.addKeyword(PvlKeyword("PolarRadius",
            (QString) radii["PolarRadius"]));
    }

    if (!ui.WasEntered("MAP")) {
      mapGroup.addKeyword(PvlKeyword("LatitudeType", "Planetocentric"));
      mapGroup.addKeyword(PvlKeyword("LongitudeDirection", "PositiveEast"));
      mapGroup.addKeyword(PvlKeyword("LongitudeDomain", toString(360)));
      mapGroup.addKeyword(PvlKeyword("CenterLatitude", toString(0)));
      mapGroup.addKeyword(PvlKeyword("CenterLongitude", toString(0)));
    }

    double minLat = ui.GetDouble("MINLAT");
    double maxLat = ui.GetDouble("MAXLAT");
    double minLon = ui.GetDouble("MINLON");
    double maxLon = ui.GetDouble("MAXLON");
    checkLatitude(minLat, maxLat);

    mapGroup.addKeyword(PvlKeyword("MinimumLatitude", toString(minLat)), Pvl::Replace);
    mapGroup.addKeyword(PvlKeyword("MaximumLatitude", toString(maxLat)), Pvl::Replace);
    mapGroup.addKeyword(PvlKeyword("MinimumLongitude", toString(minLon)), Pvl::Replace);
    mapGroup.addKeyword(PvlKeyword("MaximumLongitude", toString(maxLon)), Pvl::Replace);

    Projection *proj = ProjectionFactory::Create(userMap);

    int lonDomain = (int) IString(proj->LongitudeDomainString());
    checkLongitude(minLon, maxLon, lonDomain);

    // Convert the Lat/Lon range to an X/Y range.
    double minX;
    double minY;
    double maxX;
    double maxY;
    bool foundRange = proj->XYRange(minX, maxX, minY, maxY);
    if (!foundRange) {
      QString msg = "Cannot convert Lat/Long range to an X/Y range";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Create the control net to store the points in.
    cnet.SetTarget(target);
    QString networkId;
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

    equatorialRadius = toDouble(mapGroup.findKeyword("EquatorialRadius")[0]);

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
      QString msg = "A target must be specified by the [TARGET] parameter ";
      msg += "or included as a value for keyword [TargetName] in the projection file";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    QString target = ui.GetString("TARGET");
    PvlGroup radii = Projection::TargetRadii(target);
    equatorialRadius = radii["EquatorialRadius"];

    // Create the control net to store the points in.
    cnet.SetTarget(target);
    QString networkId;
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
  results += PvlKeyword("EquatorialRadius", toString(equatorialRadius));
  results += PvlKeyword("NumberControlPoints", toString(cnet.GetNumPoints()));
  Application::Log(results);

  cnet.Write(ui.GetFileName("ONET"));
}


void checkLatitude(double minLat, double maxLat) {
  if (minLat > maxLat) {
    QString msg = "MINLAT [" + toString(minLat) +
      "] is greater than MAXLAT [" + toString(maxLat) + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (minLat < -90) {
    QString msg = "MINLAT [" + toString(minLat) + "] is less than -90";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (maxLat > 90) {
    QString msg = "MAXLAT [" + toString(maxLat) + "] is greater than 90";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}


void checkLongitude(double minLon, double maxLon, int lonDomain) {
  if (minLon > maxLon) {
    double suggestedMaxLon = maxLon + lonDomain + (lonDomain - 360);
    QString msg = "MINLON [" + toString(minLon) +
      "] is greater than MAXLON [" + toString(maxLon) + "].  " +
      "If you meant to wrap around the [" + toString(lonDomain) +
      "] longitude " + "boundary, use a MAXLON of [" +
      toString(suggestedMaxLon) + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (minLon < lonDomain - 360) {
    QString msg = "MINLON [" + toString(minLon) +
      "] is less than [" + toString(lonDomain) + "] domain minimum [" +
      toString(lonDomain - 360) + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (maxLon - minLon > 360) {
    int range = (int) (maxLon - minLon - 1);
    int loops = range / 360 + 1;
    QString msg = "The specified longitude range [" + toString(minLon) +
      "] to [" + toString(maxLon) + "] seeds that same area of the target [" +
      toString(loops) + "] times";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}


// Helper function to print out mapfile to session log
void printMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  //Write map file out to the log
  Isis::Application::GuiLog(userGrp);
}

