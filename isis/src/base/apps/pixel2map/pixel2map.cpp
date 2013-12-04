#define GUIHELPERS

#include "Isis.h"

#include <QDebug>
#include <QList>
#include <QPointF>
#include <QString>

#include "Camera.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "FileList.h"
#include "IException.h"
#include "PixelIfov.h"
#include "ProcessByBrick.h"
#include "ProcessGroundPolygons.h"
#include "ProcessRubberSheet.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Target.h"

#include "pixel2map.h"

using namespace std;
using namespace Isis;

void PrintMap();
void rasterizePixel(Isis::Buffer &in);
std::vector<double> dns;

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  return helper;
}

// Global variables
ProcessGroundPolygons g_pgp;
UniversalGroundMap *g_groundMap;
int g_bands;
Camera *incam;
PixelIfov *g_pifov;

void IsisMain() {

  ProcessRubberSheet p;
  incam = NULL;
  Cube *icube;

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  FileList list;
  if (ui.GetString("FROMTYPE") == "FROM") {
    list.push_back(FileName(ui.GetFileName("FROM")));
  }
  else {
    list.read(ui.GetFileName("FROMLIST"));
  }
  if (list.size() < 1) {
    QString msg = "The list file [" + ui.GetFileName("FROMLIST") +
                  "does not contain any data";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  double newminlat, newmaxlat, newminlon, newmaxlon;
  double minlat = 90;
  double maxlat = -90;
  double minlon = 360.0;
  double maxlon = 0;
  PvlGroup camGrp;
  PvlGroup bandBinGrp;
  QString lastBandString;

  // Get the combined lat/lon range for all input cubes
  for (int i = 0; i < list.size(); i++) {
    // Open the input cube and get the camera
    CubeAttributeInput atts0(list[i]);
    icube = p.SetInputCube(list[i].toString(), atts0);
    g_bands = icube->bandCount();
    incam = icube->camera();

    // Make sure it is not the sky
    if (incam->target()->isSky()) {
      QString msg = "The image [" + list[i].toString() +
                    "] is targeting the sky, use skymap instead.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Make sure all the bands for all the files match
    if (i > 1 && atts0.bandsString() != lastBandString) {
      QString msg = "The Band numbers for all the files do not match.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    else {
      lastBandString = atts0.bandsString();
    }

    // Get the mapping group and the BandBin group
    Pvl camMap;
    incam->BasicMapping(camMap);
    camGrp = camMap.findGroup("Mapping");
    if (icube->hasGroup("BandBin")) {
      bandBinGrp = icube->group("BandBin");

    }

    incam->GroundRange(newminlat, newmaxlat, newminlon, newmaxlon, userMap);
    //set min lat/lon
    if (newminlat < minlat) {
      minlat = newminlat;
    }
    if (newminlon < minlon) {
      minlon = newminlon;
    }

    //set max lat/lon
    if (newmaxlat > maxlat) {
      maxlat = newmaxlat;
    }
    if (newmaxlon > maxlon) {
      maxlon = newmaxlon;
    }
  } //end for list.size

  camGrp.addKeyword(PvlKeyword("MinimumLatitude", toString(minlat)), Pvl::Replace);
  camGrp.addKeyword(PvlKeyword("MaximumLatitude", toString(maxlat)), Pvl::Replace);
  camGrp.addKeyword(PvlKeyword("MinimumLongitude", toString(minlon)), Pvl::Replace);
  camGrp.addKeyword(PvlKeyword("MaximumLongitude", toString(maxlon)), Pvl::Replace);


  // We want to delete the keywords we just added if the user wants the range
  // out of the mapfile, otherwise they will replace any keywords not in the
  // mapfile
  if (ui.GetString("DEFAULTRANGE") == "MAP") {
    camGrp.deleteKeyword("MinimumLatitude");
    camGrp.deleteKeyword("MaximumLatitude");
    camGrp.deleteKeyword("MinimumLongitude");
    camGrp.deleteKeyword("MaximumLongitude");
  }

  // Otherwise, remove the keywords from the map file so the camera keywords
  // will be propogated correctly
  else {
    while (userGrp.hasKeyword("MinimumLatitude")) {
      userGrp.deleteKeyword("MinimumLatitude");
    }
    while (userGrp.hasKeyword("MinimumLongitude")) {
      userGrp.deleteKeyword("MinimumLongitude");
    }
    while (userGrp.hasKeyword("MaximumLatitude")) {
      userGrp.deleteKeyword("MaximumLatitude");
    }
    while (userGrp.hasKeyword("MaximumLongitude")) {
      userGrp.deleteKeyword("MaximumLongitude");
    }
  }

  // If the user decided to enter a ground range then override
  if (ui.WasEntered("MINLON")) {
    userGrp.addKeyword(PvlKeyword("MinimumLongitude",
                                  toString(ui.GetDouble("MINLON"))), Pvl::Replace);
  }

  if (ui.WasEntered("MAXLON")) {
    userGrp.addKeyword(PvlKeyword("MaximumLongitude",
                                  toString(ui.GetDouble("MAXLON"))), Pvl::Replace);
  }

  if (ui.WasEntered("MINLAT")) {
    userGrp.addKeyword(PvlKeyword("MinimumLatitude",
                                  toString(ui.GetDouble("MINLAT"))), Pvl::Replace);
  }

  if (ui.WasEntered("MAXLAT")) {
    userGrp.addKeyword(PvlKeyword("MaximumLatitude",
                                  toString(ui.GetDouble("MAXLAT"))), Pvl::Replace);
  }

  // If they want the res. from the mapfile, delete it from the camera so
  // nothing gets overriden
  if (ui.GetString("PIXRES") == "MAP") {
    camGrp.deleteKeyword("PixelResolution");
  }
  // Otherwise, delete any resolution keywords from the mapfile so the camera
  // info is propogated over
  else if (ui.GetString("PIXRES") == "CAMERA") {
    if (userGrp.hasKeyword("Scale")) {
      userGrp.deleteKeyword("Scale");
    }
    if (userGrp.hasKeyword("PixelResolution")) {
      userGrp.deleteKeyword("PixelResolution");
    }
  }

  // Copy any defaults that are not in the user map from the camera map file
  for (int k = 0; k < camGrp.keywords(); k++) {
    if (!userGrp.hasKeyword(camGrp[k].name())) {
      userGrp += camGrp[k];
    }
  }

  // If the user decided to enter a resolution then override
  if (ui.GetString("PIXRES") == "MPP") {
    userGrp.addKeyword(PvlKeyword("PixelResolution",
                                  toString(ui.GetDouble("RESOLUTION"))),
                       Pvl::Replace);
    if (userGrp.hasKeyword("Scale")) {
      userGrp.deleteKeyword("Scale");
    }
  }
  else if (ui.GetString("PIXRES") == "PPD") {
    userGrp.addKeyword(PvlKeyword("Scale",
                                  toString(ui.GetDouble("RESOLUTION"))),
                       Pvl::Replace);
    if (userGrp.hasKeyword("PixelResolution")) {
      userGrp.deleteKeyword("PixelResolution");
    }
  }

  // See if the user want us to handle the longitude seam
  if (ui.GetString("DEFAULTRANGE") == "CAMERA" || ui.GetString("DEFAULTRANGE") == "MINIMIZE") {
// TODO: the incam below is left over from the for loop above. This must be fixed
    if (incam->IntersectsLongitudeDomain(userMap)) {
      if (ui.GetString("LONSEAM") == "AUTO") {
        if ((int) userGrp["LongitudeDomain"] == 360) {
          userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(180)),
                             Pvl::Replace);
          if (incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(360)),
                               Pvl::Replace);
          }
        }
        else {
          userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(360)),
                             Pvl::Replace);
          if (incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(180)),
                               Pvl::Replace);
          }
        }
        // Make the target info match the new longitude domain
        double minlat, maxlat, minlon, maxlon;
        incam->GroundRange(minlat, maxlat, minlon, maxlon, userMap);
        if(!ui.WasEntered("MINLAT")) {
          userGrp.addKeyword(PvlKeyword("MinimumLatitude", toString(minlat)), Pvl::Replace);
        }
        if(!ui.WasEntered("MAXLAT")) {
          userGrp.addKeyword(PvlKeyword("MaximumLatitude", toString(maxlat)), Pvl::Replace);
        }
        if(!ui.WasEntered("MINLON")) {
          userGrp.addKeyword(PvlKeyword("MinimumLongitude", toString(minlon)), Pvl::Replace);
        }
        if(!ui.WasEntered("MAXLON")) {
          userGrp.addKeyword(PvlKeyword("MaximumLongitude", toString(maxlon)), Pvl::Replace);
        }
      }

      else if (ui.GetString("LONSEAM") == "ERROR") {
        QString msg = "The image [" + ui.GetFileName("FROM") + "] crosses the " +
                     "longitude seam";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }


  Pvl pvl;
  pvl.addGroup(userGrp);
  pvl.addGroup(bandBinGrp);
  g_pgp.SetOutputCube("TO", pvl, g_bands);
  bool useCenter = true;
  if (ui.GetString("METHOD") == "CENTER") {
    useCenter = true;
  }
  else if (ui.GetString("METHOD") == " WHOLEPIXEL") {
    useCenter = false;
  }
 
  g_pgp.SetIntersectAlgorithm(useCenter);

  for (int f = 0; f < list.size(); f++) {

    Cube cube(list[f].toString(), "r");
    g_groundMap = new UniversalGroundMap(cube);
    // Loop through the input cube and get the all pixels values for all bands
    ProcessByBrick pbb;
    pbb.Progress()->SetText("Working on file:  " + list[f].toString());
    pbb.SetBrickSize(1, 1, g_bands);
    CubeAttributeInput atts0(list[f].toString());
    icube = pbb.SetInputCube(list[f].toString(), atts0, 0);
    incam = icube->camera();
    g_pifov = new PixelIfov();


    pbb.StartProcess(rasterizePixel);
    pbb.EndProcess();
  }
  g_pgp.EndProcess();


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


void rasterizePixel(Isis::Buffer &in) {

  std::vector<double>lat, lon;
  std::vector<double>dns;

  for (int i = 0; i < in.size(); i++) {
    dns.push_back(in[i]);
  }

  int l = in.Line();
  int s = in.Sample();

// TODO: This needs to be done for each band for band dependant instrumets
// Note: This can slow this down a lot

  incam->SetImage(s, l);

  // Get the IFOV in lat/lon space
  QList<QPointF> pIfovVertices = g_pifov->latLonVertices(*incam);

  int numVertices = pIfovVertices.size();
  if (numVertices > 3) {
    //  Get lat/lon for each vertex of the ifov
    for (int j = 0; j < numVertices; j++) {
      lat.push_back(pIfovVertices[j].x());
      lon.push_back(pIfovVertices[j].y());
    }

    g_pgp.Rasterize(lat, lon, dns);

    lat.clear();
    lon.clear();
  }
}
