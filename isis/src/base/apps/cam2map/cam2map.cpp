#define GUIHELPERS

#include "Isis.h"
#include "Camera.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "ProcessRubberSheet.h"
#include "IException.h"
#include "cam2map.h"
#include "Pvl.h"
#include "iString.h"
#include "PushFrameCameraDetectorMap.h"

using namespace std;
using namespace Isis;

void PrintMap();
void LoadMapRes();
void LoadCameraRes();
void LoadMapRange();
void LoadCameraRange();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  helper ["LoadMapRes"] = (void *) LoadMapRes;
  helper ["LoadCameraRes"] = (void *) LoadCameraRes;
  helper ["LoadMapRange"] = (void *) LoadMapRange;
  helper ["LoadCameraRange"] = (void *) LoadCameraRange;
  return helper;
}


// Global variables
void BandChange(const int band);
Cube *icube;
Camera *incam;

void IsisMain() {
  // We will be warping a cube
  ProcessRubberSheet p;

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();
  Pvl userMap;
  userMap.Read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping", Pvl::Traverse);

  // Open the input cube and get the camera
  icube = p.SetInputCube("FROM");
  incam = icube->getCamera();

  // Make sure it is not the sky
  if(incam->IsSky()) {
    string msg = "The image [" + ui.GetFileName("FROM") +
                 "] is targeting the sky, use skymap instead.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Get the mapping grop
  Pvl camMap;
  incam->BasicMapping(camMap);
  PvlGroup &camGrp = camMap.FindGroup("Mapping");


  // Make the target info match the user mapfile
  double minlat, maxlat, minlon, maxlon;
  incam->GroundRange(minlat, maxlat, minlon, maxlon, userMap);
  camGrp.AddKeyword(PvlKeyword("MinimumLatitude", minlat), Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MaximumLatitude", maxlat), Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MinimumLongitude", minlon), Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MaximumLongitude", maxlon), Pvl::Replace);


  // We want to delete the keywords we just added if the user wants the range
  // out of the mapfile, otherwise they will replace any keywords not in the
  // mapfile
  if(ui.GetString("DEFAULTRANGE") == "MAP" || ui.GetBoolean("MATCHMAP")) {
    camGrp.DeleteKeyword("MinimumLatitude");
    camGrp.DeleteKeyword("MaximumLatitude");
    camGrp.DeleteKeyword("MinimumLongitude");
    camGrp.DeleteKeyword("MaximumLongitude");
  }
  // Otherwise, remove the keywords from the map file so the camera keywords
  // will be propogated correctly
  else {
    while(userGrp.HasKeyword("MinimumLatitude")) {
      userGrp.DeleteKeyword("MinimumLatitude");
    }
    while(userGrp.HasKeyword("MinimumLongitude")) {
      userGrp.DeleteKeyword("MinimumLongitude");
    }
    while(userGrp.HasKeyword("MaximumLatitude")) {
      userGrp.DeleteKeyword("MaximumLatitude");
    }
    while(userGrp.HasKeyword("MaximumLongitude")) {
      userGrp.DeleteKeyword("MaximumLongitude");
    }
  }

  // If the user decided to enter a ground range then override
  if(ui.WasEntered("MINLON") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MinimumLongitude",
                                  ui.GetDouble("MINLON")), Pvl::Replace);
  }

  if(ui.WasEntered("MAXLON") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MaximumLongitude",
                                  ui.GetDouble("MAXLON")), Pvl::Replace);
  }

  if(ui.WasEntered("MINLAT") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MinimumLatitude",
                                  ui.GetDouble("MINLAT")), Pvl::Replace);
  }

  if(ui.WasEntered("MAXLAT") && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("MaximumLatitude",
                                  ui.GetDouble("MAXLAT")), Pvl::Replace);
  }

  // If they want the res. from the mapfile, delete it from the camera so
  // nothing gets overriden
  if(ui.GetString("PIXRES") == "MAP" || ui.GetBoolean("MATCHMAP")) {
    camGrp.DeleteKeyword("PixelResolution");
  }
  // Otherwise, delete any resolution keywords from the mapfile so the camera
  // info is propogated over
  else if(ui.GetString("PIXRES") == "CAMERA") {
    if(userGrp.HasKeyword("Scale")) {
      userGrp.DeleteKeyword("Scale");
    }
    if(userGrp.HasKeyword("PixelResolution")) {
      userGrp.DeleteKeyword("PixelResolution");
    }
  }

  // Copy any defaults that are not in the user map from the camera map file
  for(int k = 0; k < camGrp.Keywords(); k++) {
    if(!userGrp.HasKeyword(camGrp[k].Name())) {
      userGrp += camGrp[k];
    }
  }

  // If the user decided to enter a resolution then override
  if(ui.GetString("PIXRES") == "MPP" && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("PixelResolution",
                                  ui.GetDouble("RESOLUTION")),
                       Pvl::Replace);
    if(userGrp.HasKeyword("Scale")) {
      userGrp.DeleteKeyword("Scale");
    }
  }
  else if(ui.GetString("PIXRES") == "PPD" && !ui.GetBoolean("MATCHMAP")) {
    userGrp.AddKeyword(PvlKeyword("Scale",
                                  ui.GetDouble("RESOLUTION")),
                       Pvl::Replace);
    if(userGrp.HasKeyword("PixelResolution")) {
      userGrp.DeleteKeyword("PixelResolution");
    }
  }

  // See if the user want us to handle the longitude seam
  if((ui.GetString("DEFAULTRANGE") == "CAMERA" || ui.GetString("DEFAULTRANGE") == "MINIMIZE") &&
      !ui.GetBoolean("MATCHMAP")) {
    if(incam->IntersectsLongitudeDomain(userMap)) {
      if(ui.GetString("LONSEAM") == "AUTO") {
        if((int) userGrp["LongitudeDomain"] == 360) {
          userGrp.AddKeyword(PvlKeyword("LongitudeDomain", 180),
                             Pvl::Replace);
          if(incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.AddKeyword(PvlKeyword("LongitudeDomain", 360),
                               Pvl::Replace);
          }
        }
        else {
          userGrp.AddKeyword(PvlKeyword("LongitudeDomain", 360),
                             Pvl::Replace);
          if(incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.AddKeyword(PvlKeyword("LongitudeDomain", 180),
                               Pvl::Replace);
          }
        }
        // Make the target info match the new longitude domain
        double minlat, maxlat, minlon, maxlon;
        incam->GroundRange(minlat, maxlat, minlon, maxlon, userMap);
        if(!ui.WasEntered("MINLAT")) {
          userGrp.AddKeyword(PvlKeyword("MinimumLatitude", minlat), Pvl::Replace);
        }
        if(!ui.WasEntered("MAXLAT")) {
          userGrp.AddKeyword(PvlKeyword("MaximumLatitude", maxlat), Pvl::Replace);
        }
        if(!ui.WasEntered("MINLON")) {
          userGrp.AddKeyword(PvlKeyword("MinimumLongitude", minlon), Pvl::Replace);
        }
        if(!ui.WasEntered("MAXLON")) {
          userGrp.AddKeyword(PvlKeyword("MaximumLongitude", maxlon), Pvl::Replace);
        }
      }

      else if(ui.GetString("LONSEAM") == "ERROR") {
        string msg = "The image [" + ui.GetFileName("FROM") + "] crosses the " +
                     "longitude seam";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }

  // Use the updated label to create the output projection
  int samples, lines;
  Projection *outmap;
  bool trim;

  // Determine the image size
  if(ui.GetString("DEFAULTRANGE") == "MINIMIZE" && !ui.GetBoolean("MATCHMAP")) {
    outmap = ProjectionFactory::CreateForCube(userMap, samples, lines, *incam);
    trim = false;
  }
  else if(ui.GetString("DEFAULTRANGE") == "CAMERA" && !ui.GetBoolean("MATCHMAP")) {
    outmap = ProjectionFactory::CreateForCube(userMap, samples, lines, false);
    trim = ui.GetBoolean("TRIM");
  }
  else { // DEFAULTRANGE = MAP
    outmap = ProjectionFactory::CreateForCube(userMap, samples, lines,
             ui.GetBoolean("MATCHMAP"));
    trim = ui.GetBoolean("TRIM");
  }

  // Output the mapping group used to the Gui session log
  PvlGroup cleanMapping = outmap->Mapping();
  Application::GuiLog(cleanMapping);

  // Allocate the output cube and add the mapping labels
  Cube *ocube = p.SetOutputCube("TO", samples, lines, icube->getBandCount());

  ocube->putGroup(cleanMapping);

  // Set up the interpolator
  Interpolator *interp = NULL;
  if(ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if(ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else if(ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }

  // See if we need to deal with band dependent camera models
  if(!incam->IsBandIndependent()) {
    p.BandChange(BandChange);
  }

  //  See if center of input image projects.  If it does, force tile
  //  containing this center to be processed in ProcessRubberSheet.
  //  TODO:  WEIRD ... why is this needed ... Talk to Tracie ... JAA??
  double centerSamp = icube->getSampleCount() / 2.;
  double centerLine = icube->getLineCount() / 2.;
  if(incam->SetImage(centerSamp, centerLine)) {
    if(outmap->SetUniversalGround(incam->UniversalLatitude(),
                                  incam->UniversalLongitude())) {
      p.ForceTile(outmap->WorldX(), outmap->WorldY());
    }
  }
  // Create an alpha cube group for the output cube
  if(!ocube->hasGroup("AlphaCube")) {
    PvlGroup alpha("AlphaCube");
    alpha += PvlKeyword("AlphaSamples", icube->getSampleCount());
    alpha += PvlKeyword("AlphaLines", icube->getLineCount());
    alpha += PvlKeyword("AlphaStartingSample", 0.5);
    alpha += PvlKeyword("AlphaStartingLine", 0.5);
    alpha += PvlKeyword("AlphaEndingSample", icube->getSampleCount() + 0.5);
    alpha += PvlKeyword("AlphaEndingLine", icube->getLineCount() + 0.5);
    alpha += PvlKeyword("BetaSamples", icube->getSampleCount());
    alpha += PvlKeyword("BetaLines", icube->getLineCount());
    ocube->putGroup(alpha);
  }

  // We will need a transform class
  Transform *transform = 0;
  
  // Okay we need to decide how to apply the rubbersheeting for the transform
  // Does the user want to define how it is done?
  if (ui.GetString("WARPALGORITHM") == "FORWARDPATCH") {
    transform = new cam2mapForward(icube->getSampleCount(),
                                   icube->getLineCount(), incam, samples,lines,
                                   outmap, trim);

    if (ui.WasEntered("PATCHSIZE")) {
      int patchSize = ui.GetInteger("PATCHSIZE");
      if (patchSize <= 1) patchSize = 3; // Make the patchsize reasonable
      p.setPatchParameters(1, 1, patchSize, patchSize, 
                           patchSize-1, patchSize-1);
    }

    p.processPatchTransform(*transform, *interp);
  }

  else if (ui.GetString("WARPALGORITHM") == "REVERSEPATCH") {
    transform = new cam2mapReverse(icube->getSampleCount(),
                                   icube->getLineCount(), incam, samples,lines,
                                   outmap, trim);

    if (ui.WasEntered("PATCHSIZE")) {
      int patchSize = ui.GetInteger("PATCHSIZE");
      int minPatchSize = 4;
      if (patchSize < minPatchSize) minPatchSize = patchSize;
      p.SetTiling(patchSize, minPatchSize);
    }

    p.StartProcess(*transform, *interp);
  }

  // The user didn't want to override the program smarts.
  // Handle framing cameras.  Always process using the backward
  // driven system (tfile).  
  else if (incam->GetCameraType() == Camera::Framing) {
    transform = new cam2mapReverse(icube->getSampleCount(),
                                   icube->getLineCount(), incam, samples,lines,
                                   outmap, trim);
    p.SetTiling(4, 4);
    p.StartProcess(*transform, *interp);
  }

  // The user didn't want to override the program smarts.
  // Handle linescan cameras.  Always process using the forward
  // driven patch option. Faster and we get better orthorectification
  // 
  // TODO:  For now use the default patch size.  Need to modify
  // to determine patch size based on 1) if the limb is in the file
  // or 2) if the DTM is much coarser than the image
  else if (incam->GetCameraType() == Camera::LineScan) {
    transform = new cam2mapForward(icube->getSampleCount(),
                                   icube->getLineCount(), incam, samples,lines,
                                   outmap, trim);

    p.processPatchTransform(*transform, *interp);
  }

  // The user didn't want to override the program smarts.
  // Handle pushframe cameras.  Always process using the forward driven patch 
  // option.  It is much faster than the tfile method.  We will need to 
  // determine patch sizes based on the size of the push frame.
  // 
  // TODO: What if the user has run crop, enlarge, or shrink on the push
  // frame cube.  Things probably won't work unless they do it just right
  // TODO: What about the THEMIS VIS Camera.  Will tall narrow (128x4) patches
  // work okay?
  else if (incam->GetCameraType() == Camera::PushFrame) {
    transform = new cam2mapForward(icube->getSampleCount(),
                                   icube->getLineCount(), incam, samples,lines,
                                   outmap, trim);

    // Get the frame height
    PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *) incam->DetectorMap();
    int frameSize = dmap->frameletHeight() / dmap->LineScaleFactor();

    // Check for even/odd cube to determine starting line
    PvlGroup &instGrp = icube->getLabel()->FindGroup("Instrument", Pvl::Traverse);
    int startLine = 1;

    // Get the alpha cube group in case they cropped the image
    AlphaCube acube(*icube->getLabel());
    double betaLine = acube.AlphaLine(1.0);
    if (fabs(betaLine - 1.0) > 0.0000000001) {
      if (fabs(betaLine - (int) betaLine) > 0.00001) {
        string msg = "Input file is a pushframe camera cropped at a ";
        msg += "fractional pixel.  Can not project"; 
        throw IException(IException::User, msg, _FILEINFO_);
      }
      int offset = (((int) (betaLine + 0.5)) - 1) % frameSize;
      startLine -= offset;
    }

    if ((iString((string)instGrp["Framelets"])).UpCase() == "EVEN") {
      startLine += frameSize;
    }

    p.setPatchParameters(1, startLine, 5, frameSize,
                         4, frameSize * 2);

    p.processPatchTransform(*transform, *interp);
  }

  // The user didn't want to override the program smarts.  The other camera 
  // types have not be analyized.  This includes Radar and Point.  Continue to
  // use the reverse geom option with the default tiling hints
  else {
    transform = new cam2mapReverse(icube->getSampleCount(),
                                   icube->getLineCount(), incam, samples,lines,
                                   outmap, trim);

    int tileStart, tileEnd;
    incam->GetGeometricTilingHint(tileStart, tileEnd);
    p.SetTiling(tileStart, tileEnd);

    p.StartProcess(*transform, *interp);
  }

  // Wrap up the warping process 
  p.EndProcess();

  // add mapping to print.prt
  Application::Log(cleanMapping);

  // Cleanup
  delete outmap;
  delete transform;
  delete interp;
}

// Transform object constructor
cam2mapForward::cam2mapForward(const int inputSamples, const int inputLines,
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

// Transform method mapping input line/samps to lat/lons to output line/samps
bool cam2mapForward::Xform(double &outSample, double &outLine,
                    const double inSample, const double inLine) {
  // See if the input image coordinate converts to a lat/lon
  if (!p_incam->SetImage(inSample,inLine)) return false;

  // Does that ground coordinate work in the map projection
  double lat = p_incam->UniversalLatitude();
  double lon = p_incam->UniversalLongitude();
  if(!p_outmap->SetUniversalGround(lat,lon)) return false;

  // See if we should trim
  if((p_trim) && (p_outmap->HasGroundRange())) {
    if(p_outmap->Latitude() < p_outmap->MinimumLatitude()) return false;
    if(p_outmap->Latitude() > p_outmap->MaximumLatitude()) return false;
    if(p_outmap->Longitude() < p_outmap->MinimumLongitude()) return false;
    if(p_outmap->Longitude() > p_outmap->MaximumLongitude()) return false;
  }

  // Get the output sample/line coordinate
  outSample = p_outmap->WorldX();
  outLine = p_outmap->WorldY();

  // Make sure the point is inside the output image
  if(outSample < 0.5) return false;
  if(outLine < 0.5) return false;
  if(outSample > p_outputSamples + 0.5) return false;
  if(outLine > p_outputLines + 0.5) return false;

  // Everything is good
  return true;
}

int cam2mapForward::OutputSamples() const {
  return p_outputSamples;
}

int cam2mapForward::OutputLines() const {
  return p_outputLines;
}


// Transform object constructor
cam2mapReverse::cam2mapReverse(const int inputSamples, const int inputLines,
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
bool cam2mapReverse::Xform(double &inSample, double &inLine,
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

  if(!p_incam->SetUniversalGround(lat, lon)) return false;

  // Make sure the point is inside the input image
  if(p_incam->Sample() < 0.5) return false;
  if(p_incam->Line() < 0.5) return false;
  if(p_incam->Sample() > p_inputSamples + 0.5) return false;
  if(p_incam->Line() > p_inputLines + 0.5) return false;

  // Everything is good
  inSample = p_incam->Sample();
  inLine = p_incam->Line();

  return true;
}

int cam2mapReverse::OutputSamples() const {
  return p_outputSamples;
}

int cam2mapReverse::OutputLines() const {
  return p_outputLines;
}

void BandChange(const int band) {
  incam->SetBand(band);
}

// Helper function to print out mapfile to session log
void PrintMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping", Pvl::Traverse);

  //Write map file out to the log
  Application::GuiLog(userGrp);
}

// Helper function to get mapping resolution.
void LoadMapRes() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping", Pvl::Traverse);

  // Set resolution
  if(userGrp.HasKeyword("Scale")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION", userGrp["Scale"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES", "PPD");
  }
  else if(userGrp.HasKeyword("PixelResolution")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION", userGrp["PixelResolution"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES", "MPP");
  }
  else {
    string msg = "No resolution value found in [" + ui.GetFileName("MAP") + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}

//Helper function to get camera resolution.
void LoadCameraRes() {
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFileName("FROM");

  // Open the input cube, get the camera object, and the cam map projection
  Cube c;
  c.open(file);
  Camera *cam = c.getCamera();
  Pvl camMap;
  cam->BasicMapping(camMap);
  PvlGroup &camGrp = camMap.FindGroup("Mapping");

  ui.Clear("RESOLUTION");
  ui.PutDouble("RESOLUTION", camGrp["PixelResolution"]);

  ui.Clear("PIXRES");
  ui.PutAsString("PIXRES", "MPP");
}

//Helper function to get ground range from map file.
void LoadMapRange() {
  UserInterface &ui = Application::GetUserInterface();

  // Get map file
  Pvl userMap;
  userMap.Read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping", Pvl::Traverse);

  // Set ground range keywords that are found in mapfile
  int count = 0;
  ui.Clear("MINLAT");
  ui.Clear("MAXLAT");
  ui.Clear("MINLON");
  ui.Clear("MAXLON");
  if(userGrp.HasKeyword("MinimumLatitude")) {
    ui.PutDouble("MINLAT", userGrp["MinimumLatitude"]);
    count++;
  }
  if(userGrp.HasKeyword("MaximumLatitude")) {
    ui.PutDouble("MAXLAT", userGrp["MaximumLatitude"]);
    count++;
  }
  if(userGrp.HasKeyword("MinimumLongitude")) {
    ui.PutDouble("MINLON", userGrp["MinimumLongitude"]);
    count++;
  }
  if(userGrp.HasKeyword("MaximumLongitude")) {
    ui.PutDouble("MAXLON", userGrp["MaximumLongitude"]);
    count++;
  }

  // Set default ground range param to map
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE", "MAP");

  if(count < 4) {
    string msg = "One or more of the values for the ground range was not found";
    msg += " in [" + ui.GetFileName("MAP") + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}

//Helper function to load camera range.
void LoadCameraRange() {
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFileName("FROM");

  // Get the map projection file provided by the user
  Pvl userMap;
  userMap.Read(ui.GetFileName("MAP"));

  // Open the input cube, get the camera object, and the cam map projection
  Cube c;
  c.open(file);
  Camera *cam = c.getCamera();

  // Make the target info match the user mapfile
  double minlat, maxlat, minlon, maxlon;
  cam->GroundRange(minlat, maxlat, minlon, maxlon, userMap);

  // Set ground range parameters in UI
  ui.Clear("MINLAT");
  ui.PutDouble("MINLAT", minlat);
  ui.Clear("MAXLAT");
  ui.PutDouble("MAXLAT", maxlat);
  ui.Clear("MINLON");
  ui.PutDouble("MINLON", minlon);
  ui.Clear("MAXLON");
  ui.PutDouble("MAXLON", maxlon);

  // Set default ground range param to camera
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE", "CAMERA");
}

