#define GUIHELPERS

#include "Isis.h"
#include "ProcessRubberSheet.h"
#include "ProjectionFactory.h"
#include "Projection.h"
#include "map2map.h"

using namespace std;
using namespace Isis;

void PrintMap ();
void LoadMapRange();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["PrintMap"] = (void*) PrintMap;
  helper ["LoadMapRange"] = (void*) LoadMapRange;
  return helper;
}

void IsisMain() {
  // We will be warping a cube
  ProcessRubberSheet p;

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();
  Pvl userPvl(ui.GetFilename("MAP"));
  PvlGroup &userMappingGrp = userPvl.FindGroup("Mapping",Pvl::Traverse);

  // Open the input cube and get the projection
  Cube *icube = p.SetInputCube ("FROM");

  // Get the mapping group
  PvlGroup fromMappingGrp = icube->GetGroup("Mapping");
  Projection *inproj = icube->Projection();
  PvlGroup outMappingGrp = fromMappingGrp;

  // If the default range is FROM, then wipe out any range data in user mapping file
  if(ui.GetString("DEFAULTRANGE").compare("FROM") == 0 && !ui.GetBoolean("MATCHMAP")) {
    if(userMappingGrp.HasKeyword("MinimumLatitude")) {
      userMappingGrp.DeleteKeyword("MinimumLatitude");
    }

    if(userMappingGrp.HasKeyword("MaximumLatitude")) {
      userMappingGrp.DeleteKeyword("MaximumLatitude");
    }

    if(userMappingGrp.HasKeyword("MinimumLongitude")) {
      userMappingGrp.DeleteKeyword("MinimumLongitude");
    }

    if(userMappingGrp.HasKeyword("MaximumLongitude")) {
      userMappingGrp.DeleteKeyword("MaximumLongitude");
    }
  }

  // Deal with user overrides entered in the GUI. Do this by changing the user's mapping group, which
  // will then overlay anything in the output mapping group.
  if(ui.WasEntered("MINLAT") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.AddKeyword( PvlKeyword("MinimumLatitude", ui.GetDouble("MINLAT")), Pvl::Replace );
  }

  if(ui.WasEntered("MAXLAT") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.AddKeyword( PvlKeyword("MaximumLatitude", ui.GetDouble("MAXLAT")), Pvl::Replace );
  }

  if(ui.WasEntered("MINLON") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.AddKeyword( PvlKeyword("MinimumLongitude", ui.GetDouble("MINLON")), Pvl::Replace );
  }

  if(ui.WasEntered("MAXLON") && !ui.GetBoolean("MATCHMAP")) {
    userMappingGrp.AddKeyword( PvlKeyword("MaximumLongitude", ui.GetDouble("MAXLON")), Pvl::Replace );
  }

  /**
   * If the user is changing from positive east to positive west, or vice-versa, the output minimum is really
   * the input maximum. However, the user mapping group must be left unaffected (an input minimum must be the
   * output minimum). To accomplish this, we swap the minimums/maximums in the output group ahead of time. This
   * causes the minimums and maximums to correlate to the output minimums and maximums. That way when we copy
   * the user mapping group into the output group a mimimum overrides a minimum and a maximum overrides a maximum.
   */
  bool sameDirection = true;
  if(userMappingGrp.HasKeyword("LongitudeDirection")) {
    if(((string)userMappingGrp["LongitudeDirection"]).compare(fromMappingGrp["LongitudeDirection"]) != 0) {
      sameDirection = false;
    }
  }

  // Since the out mapping group came from the from mapping group, which came from a valid cube,
  // we can assume both min/max lon exists if min longitude exists.
  if(!sameDirection && outMappingGrp.HasKeyword("MinimumLongitude")) {
    double minLon = outMappingGrp["MinimumLongitude"];
    double maxLon = outMappingGrp["MaximumLongitude"];

    outMappingGrp["MaximumLongitude"] = minLon;
    outMappingGrp["MinimumLongitude"] = maxLon;
  }

  if(ui.GetString("PIXRES").compare("FROM") == 0 && !ui.GetBoolean("MATCHMAP")) {
    // Resolution will be in fromMappingGrp and outMappingGrp at this time
    //   delete from user mapping grp
    if(userMappingGrp.HasKeyword("Scale")) {
      userMappingGrp.DeleteKeyword("Scale");
    }

    if(userMappingGrp.HasKeyword("PixelResolution")) {
      userMappingGrp.DeleteKeyword("PixelResolution");
    }
  }
  else if(ui.GetString("PIXRES").compare("MAP") == 0 || ui.GetBoolean("MATCHMAP")) {
    // Resolution will be in userMappingGrp - delete all others
    if(outMappingGrp.HasKeyword("Scale")) {
      outMappingGrp.DeleteKeyword("Scale");
    }

    if(outMappingGrp.HasKeyword("PixelResolution")) {
      outMappingGrp.DeleteKeyword("PixelResolution");
    }

    if(fromMappingGrp.HasKeyword("Scale")); {
      fromMappingGrp.DeleteKeyword("Scale");
    }

    if(fromMappingGrp.HasKeyword("PixelResolution")) {
      fromMappingGrp.DeleteKeyword("PixelResolution");
    }
  }
  else if(ui.GetString("PIXRES").compare("MPP") == 0) {
    // Resolution specified - delete all and add to outMappingGrp
    if(outMappingGrp.HasKeyword("Scale")) {
      outMappingGrp.DeleteKeyword("Scale");
    }

    if(outMappingGrp.HasKeyword("PixelResolution")) {
      outMappingGrp.DeleteKeyword("PixelResolution");
    }

    if(fromMappingGrp.HasKeyword("Scale")) {
      fromMappingGrp.DeleteKeyword("Scale");
    }

    if(fromMappingGrp.HasKeyword("PixelResolution")) {
      fromMappingGrp.DeleteKeyword("PixelResolution");
    }

    if(userMappingGrp.HasKeyword("Scale")) {
      userMappingGrp.DeleteKeyword("Scale");
    }

    if(userMappingGrp.HasKeyword("PixelResolution")) {
      userMappingGrp.DeleteKeyword("PixelResolution");
    }

    outMappingGrp.AddKeyword(PvlKeyword("PixelResolution", ui.GetDouble("RESOLUTION"), "meters/pixel"), Pvl::Replace);
  }
  else if(ui.GetString("PIXRES").compare("PPD") == 0) {
    // Resolution specified - delete all and add to outMappingGrp
    if(outMappingGrp.HasKeyword("Scale")) {
      outMappingGrp.DeleteKeyword("Scale");
    }

    if(outMappingGrp.HasKeyword("PixelResolution")) {
      outMappingGrp.DeleteKeyword("PixelResolution");
    }

    if(fromMappingGrp.HasKeyword("Scale")) {
      fromMappingGrp.DeleteKeyword("Scale");
    }

    if(fromMappingGrp.HasKeyword("PixelResolution")) {
      fromMappingGrp.DeleteKeyword("PixelResolution");
    }

    if(userMappingGrp.HasKeyword("Scale")) {
      userMappingGrp.DeleteKeyword("Scale");
    }

    if(userMappingGrp.HasKeyword("PixelResolution")) {
      userMappingGrp.DeleteKeyword("PixelResolution");
    }

    outMappingGrp.AddKeyword(PvlKeyword("Scale", ui.GetDouble("RESOLUTION"), "pixels/degree"), Pvl::Replace);
  }

  // Rotation will NOT Propagate
  if(outMappingGrp.HasKeyword("Rotation")) {
    outMappingGrp.DeleteKeyword("Rotation");
  }


  /**
   * The user specified map template file overrides what ever is in the
   * cube's mapping group.
   */
  for(int keyword = 0; keyword < userMappingGrp.Keywords(); keyword ++) {
    outMappingGrp.AddKeyword(userMappingGrp[keyword], Pvl::Replace);
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

    for(int index = 0; index < longitudes.Keywords(); index ++) {
      if(!userMappingGrp.HasKeyword(longitudes[index].Name())) {
        // use the from domain because that's where our values are coming from
        if(((string)userMappingGrp["LongitudeDirection"]).compare("PositiveEast") == 0) {
          outMappingGrp[longitudes[index].Name()] =
            Projection::ToPositiveEast(outMappingGrp[longitudes[index].Name()], outMappingGrp["LongitudeDomain"]);
        }
        else {
          outMappingGrp[longitudes[index].Name()] =
            Projection::ToPositiveWest(outMappingGrp[longitudes[index].Name()], outMappingGrp["LongitudeDomain"]);
        }
      }
    }
  }

  // The minimum/maximum longitudes should be in order now. However, if the user entered a
  // maximum that was lower than the minimum, or a minimum that was higher than the maximum this
  // may still fail. Let it throw an error when we instantiate the projection.

  // Second, longitude domain
  if(userMappingGrp.HasKeyword("LongitudeDomain")) { // user set a new domain?
    if((int)userMappingGrp["LongitudeDomain"] != (int)fromMappingGrp["LongitudeDomain"]) { // new domain different?

      PvlGroup longitudes = inproj->MappingLongitudes();

      for(int index = 0; index < longitudes.Keywords(); index ++) {
        if(!userMappingGrp.HasKeyword(longitudes[index].Name())) {
          if((int)userMappingGrp["LongitudeDomain"] == 180) {
            outMappingGrp[longitudes[index].Name()] = Projection::To180Domain(outMappingGrp[longitudes[index].Name()]);
          }
          else {
            outMappingGrp[longitudes[index].Name()] = Projection::To360Domain(outMappingGrp[longitudes[index].Name()]);
          }
        }
      }

    }
  }

  // Third, planetographic/planetocentric
  if(userMappingGrp.HasKeyword("LatitudeType")) { // user set a new domain?
    if(((string)userMappingGrp["LatitudeType"]).compare(fromMappingGrp["LatitudeType"]) != 0) { // new lat type different?

      PvlGroup latitudes = inproj->MappingLatitudes();

      for(int index = 0; index < latitudes.Keywords(); index ++) {
        if(!userMappingGrp.HasKeyword(latitudes[index].Name())) {
          if(((string)userMappingGrp["LatitudeType"]).compare("Planetographic") == 0) {
            outMappingGrp[latitudes[index].Name()] = Projection::ToPlanetographic(
                                                      (double)fromMappingGrp[latitudes[index].Name()],
                                                      (double)fromMappingGrp["EquatorialRadius"],
                                                      (double)fromMappingGrp["PolarRadius"]);
          }
          else {
            outMappingGrp[latitudes[index].Name()] = Projection::ToPlanetocentric(
                                                      (double)fromMappingGrp[latitudes[index].Name()],
                                                      (double)fromMappingGrp["EquatorialRadius"],
                                                      (double)fromMappingGrp["PolarRadius"]);
          }
        }
      }

    }
  }

  // If MinLon/MaxLon out of order, we weren't able to calculate the correct values
  if((double)outMappingGrp["MinimumLongitude"] >= (double)outMappingGrp["MaximumLongitude"]) {
    if(!ui.WasEntered("MINLON") || !ui.WasEntered("MAXLON")) {
      string msg = "Unable to determine the correct [MinimumLongitude,MaximumLongitude].";
      msg += " Please specify these values in the [MINLON,MAXLON] parameters";
      throw iException::Message(iException::Pvl,msg,_FILEINFO_);
    }
  }

  int samples,lines;
  Pvl mapData;
  // Copy to preserve cube labels so we can match cube size
  if (userPvl.HasObject("IsisCube")) {
    mapData = userPvl;
    mapData.FindObject("IsisCube").DeleteGroup("Mapping");
    mapData.FindObject("IsisCube").AddGroup(outMappingGrp);
  }
  else {
    mapData.AddGroup(outMappingGrp);
  }

  // *NOTE: The UpperLeftX,UpperLeftY keywords will not be used in the CreateForCube
  //   method, and they will instead be recalculated. This is correct.
  Projection *outproj = ProjectionFactory::CreateForCube(mapData,samples,lines,
                                                         ui.GetBoolean("MATCHMAP"));

  // Set up the transform object which will simply map
  // output line/samps -> output lat/lons -> input line/samps
  Transform *transform = new map2map (icube->Samples(),
                                       icube->Lines(),
                                       icube->Projection(),
                                       samples,
                                       lines,
                                       outproj,
                                       ui.GetBoolean("TRIM"));

  // Allocate the output cube and add the mapping labels
  Cube *ocube = p.SetOutputCube ("TO", transform->OutputSamples(),
                                            transform->OutputLines(),
                                            icube->Bands());

  PvlGroup cleanOutGrp = outproj->Mapping();

  // ProjectionFactory::CreateForCube updated mapData to have the correct
  //   upperleftcornerx, upperleftcornery, scale and resolution. Use these
  //   updated numbers.
  cleanOutGrp.AddKeyword(mapData.FindGroup("Mapping",Pvl::Traverse)["UpperLeftCornerX"], Pvl::Replace);
  cleanOutGrp.AddKeyword(mapData.FindGroup("Mapping",Pvl::Traverse)["UpperLeftCornerY"], Pvl::Replace);
  cleanOutGrp.AddKeyword(mapData.FindGroup("Mapping",Pvl::Traverse)["Scale"], Pvl::Replace);
  cleanOutGrp.AddKeyword(mapData.FindGroup("Mapping",Pvl::Traverse)["PixelResolution"], Pvl::Replace);

  ocube->PutGroup(cleanOutGrp);

  // Set up the interpolator
  Interpolator *interp;
  if (ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if (ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else if (ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }
  else {
    string msg = "Unknow value for INTERP [" + ui.GetString("INTERP") + "]";
    throw iException::Message(iException::Programmer,msg,_FILEINFO_);
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
map2map::map2map (const int inputSamples, const int inputLines, Projection *inmap,
              const int outputSamples, const int outputLines, Projection *outmap,
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

    p_inputWorldSize = abs(worldEnd - worldStart)*2;
  }
}

// Transform method mapping output line/samps to lat/lons to input line/samps
bool map2map::Xform (double &inSample, double &inLine,
                       const double outSample, const double outLine) {
  // See if the output image coordinate converts to lat/lon
  if (!p_outmap->SetWorld(outSample,outLine)) return false;

  // See if we should trim
  if ((p_trim) && (p_outmap->HasGroundRange())) {
    if (p_outmap->Latitude() < p_outmap->MinimumLatitude()) return false;
    if (p_outmap->Latitude() > p_outmap->MaximumLatitude()) return false;
    if (p_outmap->Longitude() < p_outmap->MinimumLongitude()) return false;
    if (p_outmap->Longitude() > p_outmap->MaximumLongitude()) return false;
  }

  // Get the universal lat/lon and see if it can be converted to input line/samp
  double lat = p_outmap->UniversalLatitude();
  double lon = p_outmap->UniversalLongitude();
  if (!p_inmap->SetUniversalGround(lat,lon)) return false;

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
  if (inSample < 0.5) return false;
  if (inLine < 0.5) return false;
  if (inSample > p_inputSamples + 0.5) return false;
  if (inLine > p_inputLines + 0.5) return false;

  // Everything is good
  return true;
}

int map2map::OutputSamples () const {
  return p_outputSamples;
}

int map2map::OutputLines () const {
  return p_outputLines;
}


// Helper function to print out mapfile to session log
void PrintMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  //Write map file out to the log
  Isis::Application::GuiLog(userGrp);
}

void LoadMapRange() {
  UserInterface &ui = Application::GetUserInterface();

  // Get map file
  Pvl userMap;

  try {
    userMap.Read(ui.GetFilename("MAP"));
  }
  catch (iException &e) {
    e.Clear();
  }

  // Get input cube
  Pvl fromMap;

  try {
    fromMap.Read(ui.GetFilename("FROM"));
  }
  catch (iException &e) {
    e.Clear();
  }

  // Try to get the mapping groups
  PvlGroup fromMapping("Mapping");

  try {
    fromMapping = fromMap.FindGroup("Mapping", Pvl::Traverse);
  }
  catch (iException &e) {
    e.Clear();
  }

  PvlGroup userMapping("Mapping");

  try {
    userMapping = userMap.FindGroup("Mapping", Pvl::Traverse);
  }
  catch (iException &e) {
    e.Clear();
  }

  // Do conversions on from map

  // Longitude conversions first
  if(userMapping.HasKeyword("LongitudeDirection")) {
    if(((string)userMapping["LongitudeDirection"]).compare(fromMapping["LongitudeDirection"]) != 0) {
      double minLon = fromMapping["MinimumLongitude"];
      double maxLon = fromMapping["MaximumLongitude"];
      int domain = fromMapping["LongitudeDomain"];

      if(userMapping.HasKeyword("LongitudeDomain")) {
        domain = userMapping["LongitudeDomain"];
      }

      if((string)userMapping["LongitudeDirection"] == "PositiveEast") {
        fromMapping["MaximumLongitude"] = Projection::ToPositiveEast(minLon, domain);
        fromMapping["MinimumLongitude"] = Projection::ToPositiveEast(maxLon, domain);
      }
      else if((string)userMapping["LongitudeDirection"] == "PositiveWest") {
        fromMapping["MaximumLongitude"] = Projection::ToPositiveWest(minLon, domain);
        fromMapping["MinimumLongitude"] = Projection::ToPositiveWest(maxLon, domain);
      }
    }
  }

  // Latitude conversions now
  if(userMapping.HasKeyword("LatitudeType")) { // user set a new domain?
    if(((string)userMapping["LatitudeType"]).compare(fromMapping["LatitudeType"]) != 0) { // new lat type different?
      if(((string)userMapping["LatitudeType"]).compare("Planetographic") == 0) {
        fromMapping["MinimumLatitude"] = Projection::ToPlanetographic(
                                                  (double)fromMapping["MinimumLatitude"],
                                                  (double)fromMapping["EquatorialRadius"],
                                                  (double)fromMapping["PolarRadius"]);
        fromMapping["MaximumLatitude"] = Projection::ToPlanetographic(
                                                  (double)fromMapping["MaximumLatitude"],
                                                  (double)fromMapping["EquatorialRadius"],
                                                  (double)fromMapping["PolarRadius"]);
      }
      else {
        fromMapping["MinimumLatitude"] = Projection::ToPlanetocentric(
                                                  (double)fromMapping["MinimumLatitude"],
                                                  (double)fromMapping["EquatorialRadius"],
                                                  (double)fromMapping["PolarRadius"]);
        fromMapping["MaximumLatitude"] = Projection::ToPlanetocentric(
                                                  (double)fromMapping["MaximumLatitude"],
                                                  (double)fromMapping["EquatorialRadius"],
                                                  (double)fromMapping["PolarRadius"]);
      }
    }
  }

  // Failed at longitudes, use our originals!
  if((double)fromMapping["MinimumLongitude"] >= (double)fromMapping["MaximumLongitude"]) {
    try {
      fromMapping["MinimumLongitude"] = fromMap.FindGroup("Mapping", Pvl::Traverse)["MinimumLongitude"];
      fromMapping["MaximumLongitude"] = fromMap.FindGroup("Mapping", Pvl::Traverse)["MaximumLongitude"];
    }
    catch (iException &e) {
      e.Clear();
    }
  }

  // Overlay lat/lons in map file (if DEFAULTRANGE=MAP)
  if(ui.GetString("DEFAULTRANGE") == "MAP") {
    if(userMapping.HasKeyword("MinimumLatitude")) {
      fromMapping["MinimumLatitude"] = userMapping["MinimumLatitude"];
    }

    if(userMapping.HasKeyword("MaximumLatitude")) {
      fromMapping["MaximumLatitude"] = userMapping["MaximumLatitude"];
    }

    if(userMapping.HasKeyword("MinimumLongitude")) {
      fromMapping["MinimumLongitude"] = userMapping["MinimumLongitude"];
    }

    if(userMapping.HasKeyword("MaximumLongitude")) {
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
