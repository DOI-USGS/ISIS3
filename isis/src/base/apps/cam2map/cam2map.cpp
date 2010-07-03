#define GUIHELPERS

#include "Isis.h"
#include "Camera.h"
#include "ProjectionFactory.h"
#include "ProcessRubberSheet.h"
#include "iException.h"
#include "cam2map.h"

using namespace std;
using namespace Isis;

void PrintMap ();
void LoadMapRes ();
void LoadCameraRes ();
void LoadMapRange ();
void LoadCameraRange ();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["PrintMap"] = (void*) PrintMap;
  helper ["LoadMapRes"] = (void*) LoadMapRes;
  helper ["LoadCameraRes"] = (void*) LoadCameraRes;
  helper ["LoadMapRange"] = (void*) LoadMapRange;
  helper ["LoadCameraRange"] = (void*) LoadCameraRange;
  return helper;
}


// Global variables
void BandChange (const int band);
Cube *icube;
Camera *incam;

void IsisMain() {
  // We will be warping a cube
  ProcessRubberSheet p;

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  // Open the input cube and get the camera
  icube = p.SetInputCube ("FROM");
  incam = icube->Camera();

  // Make sure it is not the sky
  if (incam->IsSky()) {
    string msg = "The image [" + ui.GetFilename("FROM") +
                 "] is targeting the sky, use skymap instead.";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  // Get the mapping grop
  Pvl camMap;
  incam->BasicMapping(camMap);
  PvlGroup &camGrp = camMap.FindGroup("Mapping");


  // Make the target info match the user mapfile
  double minlat,maxlat,minlon,maxlon;
  incam->GroundRange(minlat,maxlat,minlon,maxlon,userMap);
  camGrp.AddKeyword(PvlKeyword("MinimumLatitude",minlat),Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MaximumLatitude",maxlat),Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MinimumLongitude",minlon),Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MaximumLongitude",maxlon),Pvl::Replace);


  // We want to delete the keywords we just added if the user wants the range
  // out of the mapfile, otherwise they will replace any keywords not in the
  // mapfile
  if (ui.GetString("DEFAULTRANGE") == "MAP" || ui.GetBoolean("MATCHMAP")) {
    camGrp.DeleteKeyword("MinimumLatitude");
    camGrp.DeleteKeyword("MaximumLatitude");
    camGrp.DeleteKeyword("MinimumLongitude");
    camGrp.DeleteKeyword("MaximumLongitude");
  }
  // Otherwise, remove the keywords from the map file so the camera keywords
  // will be propogated correctly
  else {
    while (userGrp.HasKeyword("MinimumLatitude")) {
      userGrp.DeleteKeyword("MinimumLatitude");
    }
    while (userGrp.HasKeyword("MinimumLongitude")) {
      userGrp.DeleteKeyword("MinimumLongitude");
    }
    while (userGrp.HasKeyword("MaximumLatitude")) {
      userGrp.DeleteKeyword("MaximumLatitude");
    }
    while (userGrp.HasKeyword("MaximumLongitude")) {
      userGrp.DeleteKeyword("MaximumLongitude");
    }
  }

  // If the user decided to enter a ground range then override
  if (ui.WasEntered("MINLON") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MinimumLongitude",
                                      ui.GetDouble("MINLON")),Pvl::Replace);
  }

  if (ui.WasEntered("MAXLON") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MaximumLongitude",
                                      ui.GetDouble("MAXLON")),Pvl::Replace);
  }

  if (ui.WasEntered("MINLAT") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MinimumLatitude",
                                      ui.GetDouble("MINLAT")),Pvl::Replace);
  }

  if (ui.WasEntered("MAXLAT") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MaximumLatitude",
                                      ui.GetDouble("MAXLAT")),Pvl::Replace);
  }

  // If they want the res. from the mapfile, delete it from the camera so
  // nothing gets overriden
  if (ui.GetString("PIXRES") == "MAP" || ui.GetBoolean("MATCHMAP")) {
    camGrp.DeleteKeyword("PixelResolution");
  }
  // Otherwise, delete any resolution keywords from the mapfile so the camera
  // info is propogated over
  else if (ui.GetString("PIXRES") == "CAMERA") {
    if (userGrp.HasKeyword("Scale")) {
      userGrp.DeleteKeyword("Scale");
    }
    if (userGrp.HasKeyword("PixelResolution")) {
      userGrp.DeleteKeyword("PixelResolution");
    }
  }

  // Copy any defaults that are not in the user map from the camera map file
  for (int k=0; k<camGrp.Keywords(); k++) {
    if (!userGrp.HasKeyword(camGrp[k].Name())) {
      userGrp += camGrp[k];
    }
  }

  // If the user decided to enter a resolution then override
  if (ui.GetString("PIXRES") == "MPP" && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("PixelResolution",
                                      ui.GetDouble("RESOLUTION")),
                                      Pvl::Replace);
    if (userGrp.HasKeyword("Scale")) {
      userGrp.DeleteKeyword("Scale");
    }
  }
  else if (ui.GetString("PIXRES") == "PPD" && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("Scale",
                                      ui.GetDouble("RESOLUTION")),
                                      Pvl::Replace);
    if (userGrp.HasKeyword("PixelResolution")) {
      userGrp.DeleteKeyword("PixelResolution");
    }
  }

  // See if the user want us to handle the longitude seam
  if ((ui.GetString("DEFAULTRANGE") == "CAMERA" || ui.GetString("DEFAULTRANGE") == "MINIMIZE") && 
      !ui.GetBoolean("MATCHMAP")) {
    if (incam->IntersectsLongitudeDomain(userMap)) {
      if (ui.GetString("LONSEAM") == "AUTO") {
        if ((int) userGrp["LongitudeDomain"] == 360) {
          userGrp.AddKeyword(PvlKeyword("LongitudeDomain",180),
                                            Pvl::Replace);
          if (incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.AddKeyword(PvlKeyword("LongitudeDomain",360),
                                            Pvl::Replace);
          }
        }
        else {
          userGrp.AddKeyword(PvlKeyword("LongitudeDomain",360),
                                            Pvl::Replace);
          if (incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.AddKeyword(PvlKeyword("LongitudeDomain",180),
                                              Pvl::Replace);
          }
        }
        // Make the target info match the new longitude domain
        double minlat,maxlat,minlon,maxlon;
        incam->GroundRange(minlat,maxlat,minlon,maxlon,userMap);
        if( !ui.WasEntered("MINLAT") ) {
          userGrp.AddKeyword(PvlKeyword("MinimumLatitude",minlat),Pvl::Replace);
        }
        if( !ui.WasEntered("MAXLAT") ) {
          userGrp.AddKeyword(PvlKeyword("MaximumLatitude",maxlat),Pvl::Replace);
        }
        if( !ui.WasEntered("MINLON") ) {
          userGrp.AddKeyword(PvlKeyword("MinimumLongitude",minlon),Pvl::Replace);
        }
        if( !ui.WasEntered("MAXLON") ) {
          userGrp.AddKeyword(PvlKeyword("MaximumLongitude",maxlon),Pvl::Replace);
        }
      }

      else if (ui.GetString("LONSEAM") == "ERROR") {
        string msg = "The image [" + ui.GetFilename("FROM") + "] crosses the " +
                                                              "longitude seam";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
    }
  }

  // Use the updated label to create the output projection
  int samples,lines;
  Projection *outmap;
  bool trim;

  // Determine the image size
  if (ui.GetString("DEFAULTRANGE") == "MINIMIZE" && !ui.GetBoolean("MATCHMAP")) {
    outmap = ProjectionFactory::CreateForCube(userMap,samples,lines,*incam);
    trim = false;
  }
  else if (ui.GetString("DEFAULTRANGE") == "CAMERA" && !ui.GetBoolean("MATCHMAP")) {
    outmap = ProjectionFactory::CreateForCube(userMap,samples,lines,false);
    trim = ui.GetBoolean("TRIM");
  }
  else { // DEFAULTRANGE = MAP
    outmap = ProjectionFactory::CreateForCube(userMap,samples,lines,
                                              ui.GetBoolean("MATCHMAP"));
    trim = ui.GetBoolean("TRIM");
  }

  int tileStart, tileEnd;
  incam->GetGeometricTilingHint(tileStart, tileEnd);
  p.SetTiling(tileStart, tileEnd);

  // Output the mapping group used to the Gui session log
  Application::GuiLog(userMap);

  // Set up the transform object which will simply map
  // output line/samps -> output lat/lons -> input line/samps
  Transform *transform = new cam2map (icube->Samples(),
                                          icube->Lines(),
                                          incam,
                                          samples,
                                          lines,
                                          outmap,
                                          trim);

  // Allocate the output cube and add the mapping labels
  Cube *ocube = p.SetOutputCube ("TO", transform->OutputSamples(),
                                            transform->OutputLines(),
                                            icube->Bands());
  ocube->PutGroup(userGrp);

  // Set up the interpolator
  Interpolator *interp = NULL;
  if (ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if (ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else if (ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }

  // See if we need to deal with band dependent camera models
  if (!incam->IsBandIndependent()) {
    p.BandChange(BandChange);
  }

  //  See if center of input image projects.  If it does, force tile
  //  containing this center to be processed in ProcessRubberSheet.
  double centerSamp = icube->Samples () / 2.;
  double centerLine = icube->Lines () / 2.;
  if (incam->SetImage(centerSamp,centerLine)) {
    if (outmap->SetUniversalGround(incam->UniversalLatitude(),
                                incam->UniversalLongitude())) {
      p.ForceTile (outmap->WorldX(),outmap->WorldY ());
    }
  }
  // Create an alpha cube group for the output cube
  if (!ocube->HasGroup("AlphaCube")) {
    PvlGroup alpha("AlphaCube");
    alpha += PvlKeyword("AlphaSamples",icube->Samples());
    alpha += PvlKeyword("AlphaLines",icube->Lines());
    alpha += PvlKeyword("AlphaStartingSample",0.5);
    alpha += PvlKeyword("AlphaStartingLine",0.5);
    alpha += PvlKeyword("AlphaEndingSample",icube->Samples()+0.5);
    alpha += PvlKeyword("AlphaEndingLine",icube->Lines()+0.5);
    alpha += PvlKeyword("BetaSamples",icube->Samples());
    alpha += PvlKeyword("BetaLines",icube->Lines());
    ocube->PutGroup(alpha);
  }

  // Warp the cube
  p.StartProcess(*transform, *interp);
  p.EndProcess();

  // add mapping to print.prt
  PvlGroup mapping = outmap->Mapping(); 
  Application::Log(mapping); 

  // Cleanup
  delete outmap;
  delete transform;
  delete interp;
}

// Transform object constructor
cam2map::cam2map (const int inputSamples, const int inputLines,
                  Camera *incam, const int outputSamples,
                  const int outputLines, Projection *outmap,
                  bool trim) {
  p_inputSamples = inputSamples;
  p_inputLines = inputLines;
  p_incam = incam;

  p_outputSamples = outputSamples;
  p_outputLines = outputLines;
  p_outmap = outmap;

  p_trim = trim;
}

// Transform method mapping output line/samps to lat/lons to input line/samps
bool cam2map::Xform (double &inSample, double &inLine,
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

  if (!p_incam->SetUniversalGround(lat,lon)) return false;

  // Make sure the point is inside the input image
  if (p_incam->Sample() < 0.5) return false;
  if (p_incam->Line() < 0.5) return false;
  if (p_incam->Sample() > p_inputSamples + 0.5) return false;
  if (p_incam->Line() > p_inputLines + 0.5) return false;

  // Everything is good
  inSample = p_incam->Sample();
  inLine = p_incam->Line();

  return true;
}

int cam2map::OutputSamples () const {
  return p_outputSamples;
}

int cam2map::OutputLines () const {
  return p_outputLines;
}

void BandChange (const int band) {
  incam->SetBand(band);
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

// Helper function to get mapping resolution.
void LoadMapRes () {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  // Set resolution
  if (userGrp.HasKeyword("Scale")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION",userGrp["Scale"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES","PPD");
  }
  else if (userGrp.HasKeyword("PixelResolution")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION",userGrp["PixelResolution"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES","MPP");
  }
  else {
    string msg = "No resolution value found in [" + ui.GetFilename("MAP") + "]";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }
}

//Helper function to get camera resolution.
void LoadCameraRes () {
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFilename("FROM");

  // Open the input cube, get the camera object, and the cam map projection
  Cube c;
  c.Open(file);
  Camera *cam = c.Camera();
  Pvl camMap;
  cam->BasicMapping(camMap);
  PvlGroup &camGrp = camMap.FindGroup("Mapping");

  ui.Clear("RESOLUTION");
  ui.PutDouble("RESOLUTION",camGrp["PixelResolution"]);

  ui.Clear("PIXRES");
  ui.PutAsString("PIXRES","MPP");
}

//Helper function to get ground range from map file.
void LoadMapRange() {
  UserInterface &ui = Application::GetUserInterface();

  // Get map file
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  // Set ground range keywords that are found in mapfile
  int count = 0;
  ui.Clear("MINLAT");
  ui.Clear("MAXLAT");
  ui.Clear("MINLON");
  ui.Clear("MAXLON");
  if (userGrp.HasKeyword("MinimumLatitude")) {
    ui.PutDouble("MINLAT",userGrp["MinimumLatitude"]);
    count++;
  }
  if (userGrp.HasKeyword("MaximumLatitude")) {
    ui.PutDouble("MAXLAT",userGrp["MaximumLatitude"]);
    count++;
  }
  if (userGrp.HasKeyword("MinimumLongitude")) {
    ui.PutDouble("MINLON",userGrp["MinimumLongitude"]);
    count++;
  }
  if (userGrp.HasKeyword("MaximumLongitude")) {
    ui.PutDouble("MAXLON",userGrp["MaximumLongitude"]);
    count++;
  }

  // Set default ground range param to map
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE","MAP");

  if (count < 4) {
    string msg = "One or more of the values for the ground range was not found";
    msg += " in [" + ui.GetFilename("MAP") + "]";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }
}

//Helper function to load camera range.
void LoadCameraRange () {
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFilename("FROM");

  // Get the map projection file provided by the user
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));

  // Open the input cube, get the camera object, and the cam map projection
  Cube c;
  c.Open(file);
  Camera *cam = c.Camera();

  // Make the target info match the user mapfile
  double minlat,maxlat,minlon,maxlon;
  cam->GroundRange(minlat,maxlat,minlon,maxlon,userMap);

  // Set ground range parameters in UI
  ui.Clear("MINLAT");
  ui.PutDouble("MINLAT",minlat);
  ui.Clear("MAXLAT");
  ui.PutDouble("MAXLAT",maxlat);
  ui.Clear("MINLON");
  ui.PutDouble("MINLON",minlon);
  ui.Clear("MAXLON");
  ui.PutDouble("MAXLON",maxlon);

  // Set default ground range param to camera
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE","CAMERA");
}

