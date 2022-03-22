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
#include "PixelFOV.h"
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

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  return helper;
}

// Global variables
ProcessGroundPolygons g_processGroundPolygons;
Camera *g_incam;
int g_numIFOVs = 0;

void IsisMain() {

  g_incam = NULL;

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  FileList list;
  if (ui.GetString("FROMTYPE") == "FROM") {
    // GetAsString will capture the entire string, including attributes
    list.push_back(FileName(ui.GetAsString("FROM")));
  }
  else {
    try {
      list.read(ui.GetFileName("FROMLIST"));
    }
    catch (IException &e) {
      throw IException(e);
    }
  }

  if (ui.GetString("FOVRANGE") == "INSTANTANEOUS") {
    g_numIFOVs = 1;
  }
  else {
    g_numIFOVs = ui.GetInteger("NUMIFOV");
  }

  double newminlat, newmaxlat, newminlon, newmaxlon;
  double minlat = 90;
  double maxlat = -90;
  double minlon = 360.0;
  double maxlon = 0;
  PvlGroup camGrp;
  QString lastBandString;

  // Get the combined lat/lon range for all input cubes
  int bands = 1;
  for (int i = 0; i < list.size(); i++) {

    // Open the input cube and get the camera
    CubeAttributeInput atts0(list[i]);
    Cube icube;
    if(atts0.bands().size() != 0) {
      vector<QString> lame = atts0.bands();
      icube.setVirtualBands(lame);
    }
    icube.open( list[i].toString() );
    bands = icube.bandCount();
    g_incam = icube.camera();

    // Make sure it is not the sky
    if (g_incam->target()->isSky()) {
      QString msg = "The image [" + list[i].toString() +
                    "] is targeting the sky, use skymap instead.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Make sure all the bands for all the files match
    if (i >= 1 && atts0.bandsString() != lastBandString) {
      QString msg = "The Band numbers for all the files do not match.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    else {
      lastBandString = atts0.bandsString();
    }

    // Get the mapping group
    Pvl camMap;
    g_incam->BasicMapping(camMap);
    camGrp = camMap.findGroup("Mapping");

    g_incam->GroundRange(newminlat, newmaxlat, newminlon, newmaxlon, userMap);
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

    // The camera will be deleted when the Cube is deleted so NULL g_incam
    g_incam = NULL;
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

    // Open the last cube and use its camera
    CubeAttributeInput atts0( list.back() );
    Cube icube;
    if(atts0.bands().size() != 0) {
      vector<QString> lame = atts0.bands();
      icube.setVirtualBands(lame);
    }
    icube.open( list.back().toString() );
    g_incam = icube.camera();

    if (g_incam->IntersectsLongitudeDomain(userMap)) {
      if (ui.GetString("LONSEAM") == "AUTO") {
        if ((int) userGrp["LongitudeDomain"] == 360) {
          userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(180)),
                             Pvl::Replace);
          if (g_incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(360)),
                               Pvl::Replace);
          }
        }
        else {
          userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(360)),
                             Pvl::Replace);
          if (g_incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.addKeyword(PvlKeyword("LongitudeDomain", toString(180)),
                               Pvl::Replace);
          }
        }
        // Make the target info match the new longitude domain
        double minlat, maxlat, minlon, maxlon;
        g_incam->GroundRange(minlat, maxlat, minlon, maxlon, userMap);
        if (!ui.WasEntered("MINLAT")) {
          userGrp.addKeyword(PvlKeyword("MinimumLatitude", toString(minlat)), Pvl::Replace);
        }
        if (!ui.WasEntered("MAXLAT")) {
          userGrp.addKeyword(PvlKeyword("MaximumLatitude", toString(maxlat)), Pvl::Replace);
        }
        if (!ui.WasEntered("MINLON")) {
          userGrp.addKeyword(PvlKeyword("MinimumLongitude", toString(minlon)), Pvl::Replace);
        }
        if (!ui.WasEntered("MAXLON")) {
          userGrp.addKeyword(PvlKeyword("MaximumLongitude", toString(maxlon)), Pvl::Replace);
        }
      }

      else if (ui.GetString("LONSEAM") == "ERROR") {
        QString msg = "The image [" + ui.GetCubeName("FROM") + "] crosses the " +
                     "longitude seam";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // The camera will be deleted when the Cube is deleted so NULL g_incam
    g_incam = NULL;
  }


  Pvl pvl;
  pvl.addGroup(userGrp);

  // If there is only one input cube, we need to attach an AlphaCube to the outputs.
  if (list.size() == 1) {
    Cube parent(list[0].toString());
    if (!parent.hasGroup("AlphaCube")) {
      PvlGroup alpha("AlphaCube");
      alpha += PvlKeyword("AlphaSamples", toString(parent.sampleCount()));
      alpha += PvlKeyword("AlphaLines", toString(parent.lineCount()));
      alpha += PvlKeyword("AlphaStartingSample", toString(0.5));
      alpha += PvlKeyword("AlphaStartingLine", toString(0.5));
      alpha += PvlKeyword("AlphaEndingSample", toString(parent.sampleCount() + 0.5));
      alpha += PvlKeyword("AlphaEndingLine", toString(parent.lineCount() + 0.5));
      alpha += PvlKeyword("BetaSamples", toString(parent.sampleCount()));
      alpha += PvlKeyword("BetaLines", toString(parent.lineCount()));
      pvl.addGroup(alpha);
    }
  }

  g_processGroundPolygons.SetStatCubes("TO", pvl, bands);

  bool useCenter = true;
  if (ui.GetString("METHOD") == "CENTER") {
    useCenter = true;
  }
  else if (ui.GetString("METHOD") == " WHOLEPIXEL") {
    useCenter = false;
  }
 
  g_processGroundPolygons.SetIntersectAlgorithm(useCenter);

  for (int f = 0; f < list.size(); f++) {

    Cube cube(list[f].toString(), "r");
    // Loop through the input cube and get the all pixels values for all bands
    ProcessByBrick processBrick;
    processBrick.Progress()->SetText("Working on file:  " + list[f].toString());
    processBrick.SetBrickSize(1, 1, bands);
    // Recall list[f] is a FileName, which stores the attributes
    CubeAttributeInput atts0(list[f]);
    Cube *icube = processBrick.SetInputCube(list[f].toString(), atts0, 0);
    g_incam = icube->camera();

    processBrick.StartProcess(rasterizePixel);
    processBrick.EndProcess();
  }
  
  // When there is only one input cube, we want to propagate IsisCube labels to output cubes
  if (list.size() == 1) {
    // Note that polygons and original labels are not propagated
    g_processGroundPolygons.PropagateLabels(list[0].toString());
    // Tell Process which tables we want to propagate
    QList<QString> tablesToPropagate;
    tablesToPropagate << "InstrumentPointing" << "InstrumentPosition" << "BodyRotation"
        << "SunPosition";
    g_processGroundPolygons.PropagateTables(list[0].toString(), tablesToPropagate);
  }
  g_processGroundPolygons.EndProcess();

  // WARNING: rasterizePixel() method alters the current state of the camera.
  // If any code is added after this point, you must call setImage to return
  // to original camera state before rasterization.

}


/**
  * Helper function to print out mapfile to session log
  */
void PrintMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  //Write map file out to the log
  Isis::Application::GuiLog(userGrp);
}


/**
  * This method uses the ProcessGroundPolygons object to rasterize each 
  * pixel in the given buffer. 
  *  
  * @param in Input ProcessByBrick buffer. 
  */
void rasterizePixel(Isis::Buffer &in) {

  std::vector<double>lat, lon;
  std::vector<double>dns;

  for (int i = 0; i < in.size(); i++) {
    dns.push_back(in[i]);
  }

  int l = in.Line();
  int s = in.Sample();

  // TODO: This needs to be done for each band for band dependant instruments
  // Note: This can slow this down a lot

  // Get the IFOVs in lat/lon space
  PixelFOV fov;
  QList< QList< QPointF > > fovVertices = fov.latLonVertices(*g_incam, l, s, g_numIFOVs);

  // loop through each ifov list
  for (int ifov = 0; ifov < fovVertices.size(); ifov++) {
    // we need at least 3 vertices for a polygon
    if (fovVertices[ifov].size() > 3) {
      //  Get lat/lon for each vertex of the ifov
      for (int point = 0; point < fovVertices[ifov].size(); point++) {
        lat.push_back(fovVertices[ifov][point].x());
        lon.push_back(fovVertices[ifov][point].y());
      }
      // rasterize this ifov and clear vectors for next ifov
      g_processGroundPolygons.Rasterize(lat, lon, dns);
      lat.clear();
      lon.clear();
    }
  }
}
