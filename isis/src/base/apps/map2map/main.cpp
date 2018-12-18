#define GUIHELPERS

#include "Isis.h"
#include "ProcessRubberSheet.h"
#include "ProjectionFactory.h"
#include "TProjection.h"
#include "map2map.h"

using namespace std;
using namespace Isis;

void PrintMap();
void LoadMapRange();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  helper ["LoadMapRange"] = (void *) LoadMapRange;
  return helper;
}

void IsisMain() {
  // We will be warping a cube
  ProcessRubberSheet p;

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();
  Pvl userPvl(ui.GetFileName("MAP"));
  PvlGroup &userMappingGrp = userPvl.findGroup("Mapping", Pvl::Traverse);

  // Open the input cube and get the projection
  Cube *icube = p.SetInputCube("FROM");

  // Get the mapping group
  PvlGroup fromMappingGrp = icube->group("Mapping");
  TProjection *inproj = (TProjection *) icube->projection();
  PvlGroup outMappingGrp = fromMappingGrp;

  // If the default range is FROM, then wipe out any range data in user mapping file
  if(ui.GetString("DEFAULTRANGE").compare("FROM") == 0 && !ui.GetBoolean("MATCHMAP")) {
    if(userMappingGrp.hasKeyword("MinimumLatitude")) {
      userMappingGrp.deleteKeyword("MinimumLatitude");
    }

    if(userMappingGrp.hasKeyword("MaximumLatitude")) {
      userMappingGrp.deleteKeyword("MaximumLatitude");
    }

    if(userMappingGrp.hasKeyword("MinimumLongitude")) {
      userMappingGrp.deleteKeyword("MinimumLongitude");
    }

    if(userMappingGrp.hasKeyword("MaximumLongitude")) {
      userMappingGrp.deleteKeyword("MaximumLongitude");
    }
  }

  // Deal with user overrides entered in the GUI. Do this by changing the user's mapping group, which
  // will then overlay anything in the output mapping group.
  if(ui.WasEntered("MINLAT") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.addKeyword(PvlKeyword("MinimumLatitude", toString(ui.GetDouble("MINLAT"))), Pvl::Replace);
  }

  if(ui.WasEntered("MAXLAT") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.addKeyword(PvlKeyword("MaximumLatitude", toString(ui.GetDouble("MAXLAT"))), Pvl::Replace);
  }

  if(ui.WasEntered("MINLON") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.addKeyword(PvlKeyword("MinimumLongitude", toString(ui.GetDouble("MINLON"))), Pvl::Replace);
  }

  if(ui.WasEntered("MAXLON") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.addKeyword(PvlKeyword("MaximumLongitude", toString(ui.GetDouble("MAXLON"))), Pvl::Replace);
  }

  /**
   * If the user is changing from positive east to positive west, or vice-versa, the output minimum is really
   * the input maximum. However, the user mapping group must be left unaffected (an input minimum must be the
   * output minimum). To accomplish this, we swap the minimums/maximums in the output group ahead of time. This
   * causes the minimums and maximums to correlate to the output minimums and maximums. That way when we copy
   * the user mapping group into the output group a mimimum overrides a minimum and a maximum overrides a maximum.
   */
  bool sameDirection = true;
  if(userMappingGrp.hasKeyword("LongitudeDirection")) {
    if(((QString)userMappingGrp["LongitudeDirection"]).compare(fromMappingGrp["LongitudeDirection"]) != 0) {
      sameDirection = false;
    }
  }

  // Since the out mapping group came from the from mapping group, which came from a valid cube,
  // we can assume both min/max lon exists if min longitude exists.
  if(!sameDirection && outMappingGrp.hasKeyword("MinimumLongitude")) {
    double minLon = outMappingGrp["MinimumLongitude"];
    double maxLon = outMappingGrp["MaximumLongitude"];

    outMappingGrp["MaximumLongitude"] = toString(minLon);
    outMappingGrp["MinimumLongitude"] = toString(maxLon);
  }

  if(ui.GetString("PIXRES").compare("FROM") == 0 && !ui.GetBoolean("MATCHMAP")) {
    // Resolution will be in fromMappingGrp and outMappingGrp at this time
    //   delete from user mapping grp
    if(userMappingGrp.hasKeyword("Scale")) {
      userMappingGrp.deleteKeyword("Scale");
    }

    if(userMappingGrp.hasKeyword("PixelResolution")) {
      userMappingGrp.deleteKeyword("PixelResolution");
    }
  }
  else if(ui.GetString("PIXRES").compare("MAP") == 0 || ui.GetBoolean("MATCHMAP")) {
    // Resolution will be in userMappingGrp - delete all others
    if(outMappingGrp.hasKeyword("Scale")) {
      outMappingGrp.deleteKeyword("Scale");
    }

    if(outMappingGrp.hasKeyword("PixelResolution")) {
      outMappingGrp.deleteKeyword("PixelResolution");
    }

    if(fromMappingGrp.hasKeyword("Scale")) {
      fromMappingGrp.deleteKeyword("Scale");
    }

    if(fromMappingGrp.hasKeyword("PixelResolution")) {
      fromMappingGrp.deleteKeyword("PixelResolution");
    }
  }
  else if(ui.GetString("PIXRES").compare("MPP") == 0) {
    // Resolution specified - delete all and add to outMappingGrp
    if(outMappingGrp.hasKeyword("Scale")) {
      outMappingGrp.deleteKeyword("Scale");
    }

    if(outMappingGrp.hasKeyword("PixelResolution")) {
      outMappingGrp.deleteKeyword("PixelResolution");
    }

    if(fromMappingGrp.hasKeyword("Scale")) {
      fromMappingGrp.deleteKeyword("Scale");
    }

    if(fromMappingGrp.hasKeyword("PixelResolution")) {
      fromMappingGrp.deleteKeyword("PixelResolution");
    }

    if(userMappingGrp.hasKeyword("Scale")) {
      userMappingGrp.deleteKeyword("Scale");
    }

    if(userMappingGrp.hasKeyword("PixelResolution")) {
      userMappingGrp.deleteKeyword("PixelResolution");
    }

    outMappingGrp.addKeyword(PvlKeyword("PixelResolution", toString(ui.GetDouble("RESOLUTION")), "meters/pixel"), Pvl::Replace);
  }
  else if(ui.GetString("PIXRES").compare("PPD") == 0) {
    // Resolution specified - delete all and add to outMappingGrp
    if(outMappingGrp.hasKeyword("Scale")) {
      outMappingGrp.deleteKeyword("Scale");
    }

    if(outMappingGrp.hasKeyword("PixelResolution")) {
      outMappingGrp.deleteKeyword("PixelResolution");
    }

    if(fromMappingGrp.hasKeyword("Scale")) {
      fromMappingGrp.deleteKeyword("Scale");
    }

    if(fromMappingGrp.hasKeyword("PixelResolution")) {
      fromMappingGrp.deleteKeyword("PixelResolution");
    }

    if(userMappingGrp.hasKeyword("Scale")) {
      userMappingGrp.deleteKeyword("Scale");
    }

    if(userMappingGrp.hasKeyword("PixelResolution")) {
      userMappingGrp.deleteKeyword("PixelResolution");
    }

    outMappingGrp.addKeyword(PvlKeyword("Scale", toString(ui.GetDouble("RESOLUTION")), "pixels/degree"), Pvl::Replace);
  }

  // Rotation will NOT Propagate
  if(outMappingGrp.hasKeyword("Rotation")) {
    outMappingGrp.deleteKeyword("Rotation");
  }


  /**
   * The user specified map template file overrides what ever is in the
   * cube's mapping group.
   */
  for(int keyword = 0; keyword < userMappingGrp.keywords(); keyword ++) {
    outMappingGrp.addKeyword(userMappingGrp[keyword], Pvl::Replace);
  }

  /**
   * Now, we have to deal with unit conversions. We convert only if the following are true:
   *   1) We used values from the input cube
   *   2) The values are longitudes or latitudes
   *   3) The map file or user-specified information uses a different measurement system than
   *        the input cube for said values.
   *
   * The data is corrected for:
   *   1) Positive east/positive west
   *   2) Longitude domain
   *   3) planetographic/planetocentric.
   */

  // First, the longitude direction
  if(!sameDirection) {
    PvlGroup longitudes = inproj->MappingLongitudes();

    for(int index = 0; index < longitudes.keywords(); index ++) {
      if(!userMappingGrp.hasKeyword(longitudes[index].name())) {
        // use the from domain because that's where our values are coming from
        if(((QString)userMappingGrp["LongitudeDirection"]).compare("PositiveEast") == 0) {
          outMappingGrp[longitudes[index].name()] = toString(
            TProjection::ToPositiveEast(outMappingGrp[longitudes[index].name()],
                                        outMappingGrp["LongitudeDomain"]));
        }
        else {
          outMappingGrp[longitudes[index].name()] = toString(
              TProjection::ToPositiveWest(outMappingGrp[longitudes[index].name()],
                                          outMappingGrp["LongitudeDomain"]));
        }
      }
    }
  }

  // Second, longitude domain
  if(userMappingGrp.hasKeyword("LongitudeDomain")) { // user set a new domain?
    if((int)userMappingGrp["LongitudeDomain"] != (int)fromMappingGrp["LongitudeDomain"]) { // new domain different?
      PvlGroup longitudes = inproj->MappingLongitudes();

      for(int index = 0; index < longitudes.keywords(); index ++) {
        if(!userMappingGrp.hasKeyword(longitudes[index].name())) {
          if((int)userMappingGrp["LongitudeDomain"] == 180) {
            outMappingGrp[longitudes[index].name()] = toString(
                TProjection::To180Domain(outMappingGrp[longitudes[index].name()]));
          }
          else {
            outMappingGrp[longitudes[index].name()] = toString(
                TProjection::To360Domain(outMappingGrp[longitudes[index].name()]));
          }
        }
      }

    }
  }

  // Third, planetographic/planetocentric
  if(userMappingGrp.hasKeyword("LatitudeType")) { // user set a new domain?
    if(((QString)userMappingGrp["LatitudeType"]).compare(fromMappingGrp["LatitudeType"]) != 0) { // new lat type different?

      PvlGroup latitudes = inproj->MappingLatitudes();

      for(int index = 0; index < latitudes.keywords(); index ++) {
        if(!userMappingGrp.hasKeyword(latitudes[index].name())) {
          if(((QString)userMappingGrp["LatitudeType"]).compare("Planetographic") == 0) {
            outMappingGrp[latitudes[index].name()] = toString(TProjection::ToPlanetographic(
                  (double)fromMappingGrp[latitudes[index].name()],
                  (double)fromMappingGrp["EquatorialRadius"],
                  (double)fromMappingGrp["PolarRadius"]));
          }
          else {
            outMappingGrp[latitudes[index].name()] = toString(TProjection::ToPlanetocentric(
                  (double)fromMappingGrp[latitudes[index].name()],
                  (double)fromMappingGrp["EquatorialRadius"],
                  (double)fromMappingGrp["PolarRadius"]));
          }
        }
      }

    }
  }

  // Try a couple equivalent longitudes to fix the ordering of min,max for border cases
  if ((double)outMappingGrp["MinimumLongitude"] >=
      (double)outMappingGrp["MaximumLongitude"]) {

    if ((QString)outMappingGrp["MinimumLongitude"] == "180.0" &&
        (int)userMappingGrp["LongitudeDomain"] == 180)
      outMappingGrp["MinimumLongitude"] = "-180";

    if ((QString)outMappingGrp["MaximumLongitude"] == "-180.0" &&
        (int)userMappingGrp["LongitudeDomain"] == 180)
      outMappingGrp["MaximumLongitude"] = "180";

    if ((QString)outMappingGrp["MinimumLongitude"] == "360.0" &&
        (int)userMappingGrp["LongitudeDomain"] == 360)
      outMappingGrp["MinimumLongitude"] = "0";

    if ((QString)outMappingGrp["MaximumLongitude"] == "0.0" &&
        (int)userMappingGrp["LongitudeDomain"] == 360)
      outMappingGrp["MaximumLongitude"] = "360";
  }

  // If MinLon/MaxLon out of order, we weren't able to calculate the correct values
  if((double)outMappingGrp["MinimumLongitude"] >= (double)outMappingGrp["MaximumLongitude"]) {
    if(!ui.WasEntered("MINLON") || !ui.WasEntered("MAXLON")) {
      QString msg = "Unable to determine the correct [MinimumLongitude,MaximumLongitude].";
      msg += " Please specify these values in the [MINLON,MAXLON] parameters";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }

  int samples, lines;
  Pvl mapData;
  // Copy to preserve cube labels so we can match cube size
  if(userPvl.hasObject("IsisCube")) {
    mapData = userPvl;
    mapData.findObject("IsisCube").deleteGroup("Mapping");
    mapData.findObject("IsisCube").addGroup(outMappingGrp);
  }
  else {
    mapData.addGroup(outMappingGrp);
  }

  // *NOTE: The UpperLeftX,UpperLeftY keywords will not be used in the CreateForCube
  //   method, and they will instead be recalculated. This is correct.
  TProjection *outproj = (TProjection *) ProjectionFactory::CreateForCube(mapData, samples, lines,
                        ui.GetBoolean("MATCHMAP"));

  // Set up the transform object which will simply map
  // output line/samps -> output lat/lons -> input line/samps
  Transform *transform = new map2map(icube->sampleCount(),
                                     icube->lineCount(),
                                     (TProjection *) icube->projection(),
                                     samples,
                                     lines,
                                     outproj,
                                     ui.GetBoolean("TRIM"));

  // Allocate the output cube and add the mapping labels
  Cube *ocube = p.SetOutputCube("TO", transform->OutputSamples(),
                                transform->OutputLines(),
                                icube->bandCount());

  PvlGroup cleanOutGrp = outproj->Mapping();

  // ProjectionFactory::CreateForCube updated mapData to have the correct
  //   upperleftcornerx, upperleftcornery, scale and resolution. Use these
  //   updated numbers.
  cleanOutGrp.addKeyword(mapData.findGroup("Mapping", Pvl::Traverse)["UpperLeftCornerX"], Pvl::Replace);
  cleanOutGrp.addKeyword(mapData.findGroup("Mapping", Pvl::Traverse)["UpperLeftCornerY"], Pvl::Replace);
  cleanOutGrp.addKeyword(mapData.findGroup("Mapping", Pvl::Traverse)["Scale"], Pvl::Replace);
  cleanOutGrp.addKeyword(mapData.findGroup("Mapping", Pvl::Traverse)["PixelResolution"], Pvl::Replace);

  ocube->putGroup(cleanOutGrp);

  // Set up the interpolator
  Interpolator *interp;
  if(ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if(ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else if(ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }
  else {
    QString msg = "Unknow value for INTERP [" + ui.GetString("INTERP") + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  // Warp the cube
  p.StartProcess(*transform, *interp);
  p.EndProcess();

  Application::Log(cleanOutGrp);

  // Cleanup
  delete transform;
  delete interp;
}

// Transform object constructor
map2map::map2map(const int inputSamples, const int inputLines, TProjection *inmap,
                 const int outputSamples, const int outputLines, TProjection *outmap,
                 bool trim) {
  p_inputSamples = inputSamples;
  p_inputLines = inputLines;
  p_inmap = inmap;

  p_outputSamples = outputSamples;
  p_outputLines = outputLines;
  p_outmap = outmap;

  p_trim = trim;

  p_inputWorldSize = 0;
  bool wrapPossible = inmap->IsEquatorialCylindrical();

  if(inmap->IsEquatorialCylindrical()) {
    // Figure out how many samples 360 degrees is
    wrapPossible = wrapPossible && inmap->SetUniversalGround(0, 0);
    int worldStart = (int)(inmap->WorldX() + 0.5);
    wrapPossible = wrapPossible && inmap->SetUniversalGround(0, 180);
    int worldEnd = (int)(inmap->WorldX() + 0.5);

    p_inputWorldSize = abs(worldEnd - worldStart) * 2;
  }
}

// Transform method mapping output line/samps to lat/lons to input line/samps
bool map2map::Xform(double &inSample, double &inLine,
                    const double outSample, const double outLine) {
  // See if the output image coordinate converts to lat/lon
  if(!p_outmap->SetWorld(outSample, outLine)) return false;

  // See if we should trim
  if((p_trim) && (p_outmap->HasGroundRange())) {
    if(p_outmap->Latitude() < p_outmap->MinimumLatitude()) return false;
    if(p_outmap->Latitude() > p_outmap->MaximumLatitude()) return false;
    if(p_outmap->Longitude() < p_outmap->MinimumLongitude()) return false;
    if(p_outmap->Longitude() > p_outmap->MaximumLongitude()) return false;
  }

  // Get the universal lat/lon and see if it can be converted to input line/samp
  double lat = p_outmap->UniversalLatitude();
  double lon = p_outmap->UniversalLongitude();
  if(!p_inmap->SetUniversalGround(lat, lon)) return false;

  inSample = p_inmap->WorldX();
  inLine = p_inmap->WorldY();

  if(p_inputWorldSize != 0) {
    // Try to correct the sample if we can,
    //   this is the simplest way to code the
    //   translation although it probably could
    //   be done in one "if"
    while(inSample < 0.5) {
      inSample += p_inputWorldSize;
    }

    while(inSample > p_inputSamples + 0.5) {
      inSample -= p_inputWorldSize;
    }
  }

  // Make sure the point is inside the input image
  if(inSample < 0.5) return false;
  if(inLine < 0.5) return false;
  if(inSample > p_inputSamples + 0.5) return false;
  if(inLine > p_inputLines + 0.5) return false;

  // Everything is good
  return true;
}

int map2map::OutputSamples() const {
  return p_outputSamples;
}

int map2map::OutputLines() const {
  return p_outputLines;
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

void LoadMapRange() {
  UserInterface &ui = Application::GetUserInterface();

  // Get map file
  Pvl userMap;

  try {
    userMap.read(ui.GetFileName("MAP"));
  }
  catch(IException &e) {
  }

  // Get input cube
  Pvl fromMap;

  try {
    fromMap.read(ui.GetFileName("FROM"));
  }
  catch(IException &e) {
  }

  // Try to get the mapping groups
  PvlGroup fromMapping("Mapping");

  try {
    fromMapping = fromMap.findGroup("Mapping", Pvl::Traverse);
  }
  catch(IException &e) {
  }

  PvlGroup userMapping("Mapping");

  try {
    userMapping = userMap.findGroup("Mapping", Pvl::Traverse);
  }
  catch(IException &e) {
  }

  // Do conversions on from map

  // Longitude conversions first
  if(userMapping.hasKeyword("LongitudeDirection")) {
    if(((QString)userMapping["LongitudeDirection"]).compare(fromMapping["LongitudeDirection"]) != 0) {
      double minLon = fromMapping["MinimumLongitude"];
      double maxLon = fromMapping["MaximumLongitude"];
      int domain = fromMapping["LongitudeDomain"];

      if(userMapping.hasKeyword("LongitudeDomain")) {
        domain = userMapping["LongitudeDomain"];
      }

      if((QString)userMapping["LongitudeDirection"] == "PositiveEast") {
        fromMapping["MaximumLongitude"] = toString(TProjection::ToPositiveEast(minLon, domain));
        fromMapping["MinimumLongitude"] = toString(TProjection::ToPositiveEast(maxLon, domain));
      }
      else if((QString)userMapping["LongitudeDirection"] == "PositiveWest") {
        fromMapping["MaximumLongitude"] = toString(TProjection::ToPositiveWest(minLon, domain));
        fromMapping["MinimumLongitude"] = toString(TProjection::ToPositiveWest(maxLon, domain));
      }
    }
  }

  // Latitude conversions now
  if(userMapping.hasKeyword("LatitudeType")) { // user set a new domain?
    if(((QString)userMapping["LatitudeType"]).compare(fromMapping["LatitudeType"]) != 0) { // new lat type different?
      if(((QString)userMapping["LatitudeType"]).compare("Planetographic") == 0) {
        fromMapping["MinimumLatitude"] = toString(TProjection::ToPlanetographic(
                                           (double)fromMapping["MinimumLatitude"],
                                           (double)fromMapping["EquatorialRadius"],
                                           (double)fromMapping["PolarRadius"]));
        fromMapping["MaximumLatitude"] = toString(TProjection::ToPlanetographic(
                                           (double)fromMapping["MaximumLatitude"],
                                           (double)fromMapping["EquatorialRadius"],
                                           (double)fromMapping["PolarRadius"]));
      }
      else {
        fromMapping["MinimumLatitude"] = toString(TProjection::ToPlanetocentric(
                                           (double)fromMapping["MinimumLatitude"],
                                           (double)fromMapping["EquatorialRadius"],
                                           (double)fromMapping["PolarRadius"]));
        fromMapping["MaximumLatitude"] = toString(TProjection::ToPlanetocentric(
                                           (double)fromMapping["MaximumLatitude"],
                                           (double)fromMapping["EquatorialRadius"],
                                           (double)fromMapping["PolarRadius"]));
      }
    }
  }

  // Failed at longitudes, use our originals!
  if((double)fromMapping["MinimumLongitude"] >= (double)fromMapping["MaximumLongitude"]) {
    try {
      fromMapping["MinimumLongitude"] = fromMap.findGroup("Mapping", Pvl::Traverse)["MinimumLongitude"];
      fromMapping["MaximumLongitude"] = fromMap.findGroup("Mapping", Pvl::Traverse)["MaximumLongitude"];
    }
    catch(IException &e) {
    }
  }

  // Overlay lat/lons in map file (if DEFAULTRANGE=MAP)
  if(ui.GetString("DEFAULTRANGE") == "MAP") {
    if(userMapping.hasKeyword("MinimumLatitude")) {
      fromMapping["MinimumLatitude"] = userMapping["MinimumLatitude"];
    }

    if(userMapping.hasKeyword("MaximumLatitude")) {
      fromMapping["MaximumLatitude"] = userMapping["MaximumLatitude"];
    }

    if(userMapping.hasKeyword("MinimumLongitude")) {
      fromMapping["MinimumLongitude"] = userMapping["MinimumLongitude"];
    }

    if(userMapping.hasKeyword("MaximumLongitude")) {
      fromMapping["MaximumLongitude"] = userMapping["MaximumLongitude"];
    }
  }

  if(ui.WasEntered("MINLAT")) {
    ui.Clear("MINLAT");
  }

  if(ui.WasEntered("MAXLAT")) {
    ui.Clear("MAXLAT");
  }

  if(ui.WasEntered("MINLON")) {
    ui.Clear("MINLON");
  }

  if(ui.WasEntered("MAXLON")) {
    ui.Clear("MAXLON");
  }

  ui.PutDouble("MINLAT", fromMapping["MinimumLatitude"]);
  ui.PutDouble("MAXLAT", fromMapping["MaximumLatitude"]);
  ui.PutDouble("MINLON", fromMapping["MinimumLongitude"]);
  ui.PutDouble("MAXLON", fromMapping["MaximumLongitude"]);
}
