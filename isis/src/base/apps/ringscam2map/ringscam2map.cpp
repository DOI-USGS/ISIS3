// SPDX-License-Identifier: CC0-1.0

#include "ringscam2map.h"

#include "AlphaCube.h"
#include "Camera.h"
#include "Distance.h"
#include "IException.h"
#include "Interpolator.h"
#include "IString.h"
#include "ProcessRubberSheet.h"
#include "ProjectionFactory.h"
#include "PushFrameCameraDetectorMap.h"
#include "Pvl.h"
#include "RingPlaneProjection.h"
#include "Target.h"
#include "Transform.h"

using namespace std;

// Global variables used by BandChange callback (outside namespace)
static Cube *g_icube = nullptr;
static Camera *g_incam = nullptr;

void BandChange(const int band);

namespace Isis {

  /**
   * Main ringscam2map function that projects a ring-plane camera image to a map projection.
   *
   * @param ui User interface with parameters
   */
  void ringscam2map(UserInterface &ui) {
    // We will be warping a cube
    ProcessRubberSheet p;

    // Get the map projection file provided by the user
    Pvl userMap;
    userMap.read(ui.GetFileName("MAP"));
    PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

    // Open the input cube and get the camera
    g_icube = p.SetInputCube("FROM");
    g_incam = g_icube->camera();

    // Make sure it is not the sky
    if(g_incam->target()->isSky()) {
      QString msg = "The image [" + ui.GetCubeName("FROM") +
                    "] is targeting the sky, use skymap instead.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Get the mapping group from the camera
    Pvl camMap;
    g_incam->basicRingMapping(camMap);
    PvlGroup &camGrp = camMap.findGroup("Mapping");

    // Make the target info match the user mapfile
    double minrad, maxrad, minaz, maxaz;
    g_incam->ringRange(minrad, maxrad, minaz, maxaz, userMap);
    camGrp.addKeyword(PvlKeyword("MinimumRingRadius", toString(minrad)), Pvl::Replace);
    camGrp.addKeyword(PvlKeyword("MaximumRingRadius", toString(maxrad)), Pvl::Replace);
    camGrp.addKeyword(PvlKeyword("MinimumRingLongitude", toString(minaz)), Pvl::Replace);
    camGrp.addKeyword(PvlKeyword("MaximumRingLongitude", toString(maxaz)), Pvl::Replace);

    // We want to delete the keywords we just added if the user wants the range
    // out of the mapfile, otherwise they will replace any keywords not in the
    // mapfile
    if (ui.GetString("DEFAULTRANGE") == "MAP" || ui.GetBoolean("MATCHMAP")) {
      camGrp.deleteKeyword("MinimumRingRadius");
      camGrp.deleteKeyword("MaximumRingRadius");
      camGrp.deleteKeyword("MinimumRingLongitude");
      camGrp.deleteKeyword("MaximumRingLongitude");
    }
    // Otherwise, remove the keywords from the map file so the camera keywords
    // will be propogated correctly
    else {
      while(userGrp.hasKeyword("MinimumRingRadius")) {
        userGrp.deleteKeyword("MinimumRingRadius");
      }
      while(userGrp.hasKeyword("MinimumRingLongitude")) {
        userGrp.deleteKeyword("MinimumRingLongitude");
      }
      while(userGrp.hasKeyword("MaximumRingRadius")) {
        userGrp.deleteKeyword("MaximumRingRadius");
      }
      while(userGrp.hasKeyword("MaximumRingLongitude")) {
        userGrp.deleteKeyword("MaximumRingLongitude");
      }
    }

    // If the user decided to enter a ground range then override
    if(ui.WasEntered("MINRINGLON")) {
      userGrp.addKeyword(PvlKeyword("MinimumRingLongitude",
                                    toString(ui.GetDouble("MINRINGLON"))), Pvl::Replace);
    }

    if(ui.WasEntered("MAXRINGLON")) {
      userGrp.addKeyword(PvlKeyword("MaximumRingLongitude",
                                    toString(ui.GetDouble("MAXRINGLON"))), Pvl::Replace);
    }

    if(ui.WasEntered("MINRINGRAD")) {
      userGrp.addKeyword(PvlKeyword("MinimumRingRadius",
                                    toString(ui.GetDouble("MINRINGRAD"))), Pvl::Replace);
    }

    if(ui.WasEntered("MAXRINGRAD")) {
      userGrp.addKeyword(PvlKeyword("MaximumRingRadius",
                                    toString(ui.GetDouble("MAXRINGRAD"))), Pvl::Replace);
    }

    // If they want the res. from the mapfile, delete it from the camera so
    // nothing gets overridden
    if(ui.GetString("PIXRES") == "MAP" || ui.GetBoolean("MATCHMAP")) {
      camGrp.deleteKeyword("PixelResolution");
    }
    // Otherwise, delete any resolution keywords from the mapfile so the camera
    // info is propagated over
    else if(ui.GetString("PIXRES") == "CAMERA") {
      if(userGrp.hasKeyword("Scale")) {
        userGrp.deleteKeyword("Scale");
      }
      if(userGrp.hasKeyword("PixelResolution")) {
        userGrp.deleteKeyword("PixelResolution");
      }
    }

    // Copy any defaults that are not in the user map from the camera map file
    for(int k = 0; k < camGrp.keywords(); k++) {
      if(!userGrp.hasKeyword(camGrp[k].name())) {
        userGrp += camGrp[k];
      }
    }

    // If the user is not matching the map file and entered a resolution, then reset this value
    if (!ui.GetBoolean("MATCHMAP")) {
      if(ui.GetString("PIXRES") == "MPP") {
        userGrp.addKeyword(PvlKeyword("PixelResolution",
                                      toString(ui.GetDouble("RESOLUTION"))),
                           Pvl::Replace);
        if(userGrp.hasKeyword("Scale")) {
          userGrp.deleteKeyword("Scale");
        }
      }
      else if(ui.GetString("PIXRES") == "PPD") {
        userGrp.addKeyword(PvlKeyword("Scale",
                                      toString(ui.GetDouble("RESOLUTION"))),
                           Pvl::Replace);
        if(userGrp.hasKeyword("PixelResolution")) {
          userGrp.deleteKeyword("PixelResolution");
        }
      }
    }

    // See if the user want us to handle the azimuth (ring longitude) seam
    // NOTE: For ringscam2map, if the user chooses to MATCHMAP, then we treat
    // the parameter ringlonseam as if it were set to "continue" (i.e. do nothing)
    if (!ui.GetBoolean("MATCHMAP")) {
      if((ui.GetString("DEFAULTRANGE") == "CAMERA" || ui.GetString("DEFAULTRANGE") == "MINIMIZE")) {
        if (g_incam->ringRange(minrad, maxrad, minaz, maxaz, userMap)) {
          if(ui.GetString("RINGLONSEAM") == "AUTO") {
            if((int) userGrp["RingLongitudeDomain"] == 360) {
              userGrp.addKeyword(PvlKeyword("RingLongitudeDomain", "180"),
                                 Pvl::Replace);
              if(g_incam->ringRange(minrad, maxrad, minaz, maxaz, userMap)) {
                // Its looks like a global image so switch back to the
                // users preference
                userGrp.addKeyword(PvlKeyword("RingLongitudeDomain", "360"),
                                   Pvl::Replace);
              }
            }
            else {
              userGrp.addKeyword(PvlKeyword("RingLongitudeDomain", "360"),
                                 Pvl::Replace);
              if(g_incam->ringRange(minrad, maxrad, minaz, maxaz, userMap)) {
                // Its looks like a global image so switch back to the
                // users preference
                userGrp.addKeyword(PvlKeyword("RingLongitudeDomain", "180"),
                                   Pvl::Replace);
              }
            }
            // Make the target info match the new azimuth (ring longitude) domain Use radius for where
            // camera expects latitude & azimuth (ring longitude) where the camera expects longitude
            double minrad, maxrad, minaz, maxaz;
            g_incam->ringRange(minrad, maxrad, minaz, maxaz, userMap);
            if(!ui.WasEntered("MINRINGRAD")) {
              userGrp.addKeyword(PvlKeyword("MinimumRingRadius", toString(minrad)), Pvl::Replace);
            }
            if(!ui.WasEntered("MAXRINGRAD")) {
              userGrp.addKeyword(PvlKeyword("MaximumRingRadius", toString(maxrad)), Pvl::Replace);
            }
            if(!ui.WasEntered("MINRINGLON")) {
              userGrp.addKeyword(PvlKeyword("MinimumRingLongitude", toString(minaz)), Pvl::Replace);
            }
            if(!ui.WasEntered("MAXRINGLON")) {
              userGrp.addKeyword(PvlKeyword("MaximumRingLongitude", toString(maxaz)), Pvl::Replace);
            }
          }
          else if(ui.GetString("RINGLONSEAM") == "ERROR") {
            QString msg = "The image [" + ui.GetCubeName("FROM") + "] crosses the " +
                         "ring longitude seam";
            throw IException(IException::User, msg, _FILEINFO_);
          }
        }
      }
    }

    // Use the updated label to create the output projection
    int samples, lines;
    RingPlaneProjection *outmap;
    bool trim;

    // Determine the image size
    if (ui.GetBoolean("MATCHMAP") || ui.GetString("DEFAULTRANGE") == "MAP") {
      outmap =  (RingPlaneProjection *) ProjectionFactory::RingsCreateForCube(userMap, samples, lines,
                                                                           ui.GetBoolean("MATCHMAP"));
      trim = ui.GetBoolean("TRIM");
    }
    else if(ui.GetString("DEFAULTRANGE") == "MINIMIZE") {
      outmap = (RingPlaneProjection *) ProjectionFactory::RingsCreateForCube(userMap, samples, lines,
                                                                             *g_incam);
      trim = false;
    }
    else { // if(ui.GetString("DEFAULTRANGE") == "CAMERA") {
      outmap =  (RingPlaneProjection *) ProjectionFactory::RingsCreateForCube(userMap, samples, lines,
                                                                              false);
      trim = ui.GetBoolean("TRIM");
    }

    // Allocate the output cube and add the mapping labels
    Cube *ocube = p.SetOutputCube("TO", samples, lines, g_icube->bandCount());

    ocube->putGroup(outmap->Mapping());

    // Set up the interpolator
    Interpolator *interp = NULL;
    if(ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
      interp = new Interpolator(Interpolator::NearestNeighborType);
    }
    else if(ui.GetString("INTERP") == "BILINEAR") {
      interp = new Interpolator(Interpolator::BiLinearType);
    }
    else {
      interp = new Interpolator(Interpolator::CubicConvolutionType);
    }

    // See if we need to deal with band dependent camera models
    if(!g_incam->IsBandIndependent()) {
      p.BandChange(BandChange);
    }

    // See if center of input image projects. If it does, force tile
    // containing this center to be processed in ProcessRubberSheet.
    double centerSamp = g_icube->sampleCount() / 2.;
    double centerLine = g_icube->lineCount() / 2.;
    if(g_incam->SetImage(centerSamp, centerLine)) {
      // Force rings data into Isis by returning ring radius for latitude
      // and azimuth (ring longitude) for longitude
      if(outmap->SetUniversalGround(g_incam->LocalRadius().meters(),
                                    g_incam->UniversalLongitude())) {
        p.ForceTile(outmap->WorldX(), outmap->WorldY());
      }
    }

    // Create an alpha cube group for the output cube
    if(!ocube->hasGroup("AlphaCube")) {
      PvlGroup alpha("AlphaCube");
      alpha += PvlKeyword("AlphaSamples", toString(g_icube->sampleCount()));
      alpha += PvlKeyword("AlphaLines", toString(g_icube->lineCount()));
      alpha += PvlKeyword("AlphaStartingSample", toString(0.5));
      alpha += PvlKeyword("AlphaStartingLine", toString(0.5));
      alpha += PvlKeyword("AlphaEndingSample", toString(g_icube->sampleCount() + 0.5));
      alpha += PvlKeyword("AlphaEndingLine", toString(g_icube->lineCount() + 0.5));
      alpha += PvlKeyword("BetaSamples", toString(g_icube->sampleCount()));
      alpha += PvlKeyword("BetaLines", toString(g_icube->lineCount()));
      ocube->putGroup(alpha);
    }

    // We will need a transform class
    Transform *transform = 0;

    // Okay we need to decide how to apply the rubbersheeting for the transform
    // Does the user want to define how it is done?
    if (ui.GetString("WARPALGORITHM") == "FORWARDPATCH") {
      transform = new ringscam2mapForward(g_icube->sampleCount(),
                                     g_icube->lineCount(), g_incam, samples,lines,
                                     outmap, trim);

      int patchSize = ui.GetInteger("PATCHSIZE");
      if (patchSize <= 1) patchSize = 3;
      p.setPatchParameters(1, 1, patchSize, patchSize,
                           patchSize-1, patchSize-1);
      p.processPatchTransform(*transform, *interp);
    }

    else if (ui.GetString("WARPALGORITHM") == "REVERSEPATCH") {
      transform = new ringscam2mapReverse(g_icube->sampleCount(),
                                     g_icube->lineCount(), g_incam, samples,lines,
                                     outmap, trim);

      int patchSize = ui.GetInteger("PATCHSIZE");
      int minPatchSize = 4;
      if (patchSize < minPatchSize) minPatchSize = patchSize;
      p.SetTiling(patchSize, minPatchSize);
      p.StartProcess(*transform, *interp);
    }

    // The user didn't want to override the program smarts.
    // Handle framing cameras.  Always process using the backward
    // driven system (tfile).
    else if (g_incam->GetCameraType() == Camera::Framing) {
      transform = new ringscam2mapReverse(g_icube->sampleCount(),
                                     g_icube->lineCount(), g_incam, samples,lines,
                                     outmap, trim);
      p.SetTiling(4, 4);
      p.StartProcess(*transform, *interp);
    }

    // The user didn't want to override the program smarts.
    // Handle linescan cameras.  Always process using the forward
    // driven patch option. Faster and we get better orthorectification
    else if (g_incam->GetCameraType() == Camera::LineScan) {
      transform = new ringscam2mapForward(g_icube->sampleCount(),
                                     g_icube->lineCount(), g_incam, samples,lines,
                                     outmap, trim);

      p.processPatchTransform(*transform, *interp);
    }

    // The user didn't want to override the program smarts.
    // Handle pushframe cameras.  Always process using the forward driven patch
    // option.  It is much faster than the tfile method.  We will need to
    // determine patch sizes based on the size of the push frame.
    else if (g_incam->GetCameraType() == Camera::PushFrame) {
      transform = new ringscam2mapForward(g_icube->sampleCount(),
                                     g_icube->lineCount(), g_incam, samples,lines,
                                     outmap, trim);

      // Get the frame height
      PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *) g_incam->DetectorMap();
      int frameSize = dmap->frameletHeight() / dmap->LineScaleFactor();

      // Check for even/odd cube to determine starting line
      PvlGroup &instGrp = g_icube->label()->findGroup("Instrument", Pvl::Traverse);
      int startLine = 1;

      // Get the alpha cube group in case they cropped the image
      AlphaCube acube(*g_icube);
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

      if (((QString)instGrp["Framelets"]).toUpper() == "EVEN") {
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
      transform = new ringscam2mapReverse(g_icube->sampleCount(),
                                     g_icube->lineCount(), g_incam, samples,lines,
                                     outmap, trim);

      int tileStart, tileEnd;
      g_incam->GetGeometricTilingHint(tileStart, tileEnd);
      p.SetTiling(tileStart, tileEnd);

      p.StartProcess(*transform, *interp);
    }

    // Wrap up the warping process
    p.EndProcess();

    // Cleanup
    delete outmap;
    delete transform;
    delete interp;

    // Reset global pointers
    g_icube = nullptr;
    g_incam = nullptr;
  }

} // namespace Isis

void BandChange(const int band) {
  g_incam->SetBand(band);
}

// Transform object constructor
ringscam2mapForward::ringscam2mapForward(const int inputSamples,
                                         const int inputLines,
                                         Camera *incam, const int outputSamples,
                                         const int outputLines,
                                         RingPlaneProjection *outmap, bool trim) {
  p_inputSamples = inputSamples;
  p_inputLines = inputLines;
  p_incam = incam;

  p_outputSamples = outputSamples;
  p_outputLines = outputLines;
  p_outmap = outmap;

  p_trim = trim;
}

// Transform method mapping input line/samps to radii/lons to output line/samps
bool ringscam2mapForward::Xform(double &outSample, double &outLine,
                    const double inSample, const double inLine) {

  // See if the input image coordinate converts to a radius/lon
  if (!p_incam->SetImage(inSample,inLine)) return false;

  // Does that ground coordinate work in the map projection
  // We will be forcing ring data to work by substituting ring radius for latitude
  // & azimuth (ring longitude) for longitude
  double rad = p_incam->LocalRadius().meters();
  double az = p_incam->UniversalLongitude();
  if(!p_outmap->SetUniversalGround(rad,az)) return false;

  // See if we should trim
  if((p_trim) && (p_outmap->HasGroundRange())) {
    if(p_outmap->RingRadius() < p_outmap->MinimumRingRadius()) return false;
    if(p_outmap->RingRadius() > p_outmap->MaximumRingRadius()) return false;
    if(p_outmap->RingLongitude() < p_outmap->MinimumRingLongitude()) return false;
    if(p_outmap->RingLongitude() > p_outmap->MaximumRingLongitude()) return false;
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

int ringscam2mapForward::OutputSamples() const {
  return p_outputSamples;
}

int ringscam2mapForward::OutputLines() const {
  return p_outputLines;
}


// Transform object constructor
ringscam2mapReverse::ringscam2mapReverse(const int inputSamples,
                                         const int inputLines, Camera *incam,
                                         const int outputSamples,
                                         const int outputLines,
                                         RingPlaneProjection *outmap, bool trim) {

  p_inputSamples = inputSamples;
  p_inputLines = inputLines;
  p_incam = incam;

  p_outputSamples = outputSamples;
  p_outputLines = outputLines;
  p_outmap = outmap;

  p_trim = trim;
}

// Transform method mapping output line/samps to rads/azs to input line/samps
bool ringscam2mapReverse::Xform(double &inSample, double &inLine,
                           const double outSample, const double outLine) {
  // See if the output image coordinate converts to lat/lon
  if(!p_outmap->SetWorld(outSample, outLine)) return false;

  // See if we should trim
 if((p_trim) && (p_outmap->HasGroundRange())) {
   if(p_outmap->RingRadius() < p_outmap->MinimumRingRadius()) return false;
   if(p_outmap->RingRadius() > p_outmap->MaximumRingRadius()) return false;
   if(p_outmap->RingLongitude() < p_outmap->MinimumRingLongitude()) return false;
   if(p_outmap->RingLongitude() > p_outmap->MaximumRingLongitude()) return false;
 }

  // Get the universal rad/az and see if it can be converted to input line/samp
  double radius = p_outmap->RingRadius();
  double az = p_outmap->UniversalRingLongitude();

  if(!p_incam->SetUniversalGround(radius, az)) return false;

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

int ringscam2mapReverse::OutputSamples() const {
  return p_outputSamples;
}

int ringscam2mapReverse::OutputLines() const {
  return p_outputLines;
}
