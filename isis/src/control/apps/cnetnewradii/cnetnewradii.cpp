#include "Isis.h"

#include <iomanip>

#include "Brick.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "Filename.h"
#include "iException.h"
#include "Interpolator.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Portal.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "UniversalGroundMap.h"

using namespace std;
using namespace Isis;

enum GetLatLon { Adjusted, Apriori };

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ControlNet cnet(ui.GetFilename("CNET"));

  // Get input DEM cube and get ground map for it
  string demFile = ui.GetFilename("MODEL");
  Cube demCube;
  demCube.open(demFile);
  UniversalGroundMap *ugm = NULL;
  try {
    ugm = new UniversalGroundMap(demCube);
  }
  catch (iException e) {
    iString msg = "Cannot initalize UniversalGroundMap for DEM cube [" +
                   demFile + "]";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  //  Use bilinear interpolation to read radius from DEM
  //   Use bilinear interpolation from dem
  Interpolator *interp = new Interpolator(Interpolator::BiLinearType);;

  //   Buffer used to read from the model
  Portal *portal = new Portal(interp->Samples(), interp->Lines(),
                              demCube.getPixelType(),
                              interp->HotSample(), interp->HotLine());;


  GetLatLon newRadiiSource;
  iString getLatLon = iString(ui.GetAsString("GETLATLON")).UpCase();
  if (getLatLon == "ADJUSTED") {
      newRadiiSource = Adjusted;
  }
  else if (getLatLon == "APRIORI") {
      newRadiiSource = Apriori;
  }
  else {
      string msg = "The value for parameter GETLATLON [";
      msg += ui.GetAsString("GETLATLON") + "] must be provided.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  int numSuccesses = 0;
  int numFailures = 0;
  int numConstrainedFixed = 0;
  string failedIDs = "";

  for (int i = 0; i < cnet.GetNumPoints() ; i++) {
    ControlPoint *cp = cnet.GetPoint(i);

    if (cp->GetType() != ControlPoint::Free) {
      numConstrainedFixed++;
      // Create Brick on samp, line to get the dn value of the pixel
      SurfacePoint surfacePt;
      if (newRadiiSource == Adjusted)
        surfacePt = cp->GetAdjustedSurfacePoint();
      else if (newRadiiSource == Apriori)
        surfacePt = cp->GetAprioriSurfacePoint();
      bool success = surfacePt.Valid();

      if (success) {
        success = ugm->SetUniversalGround(surfacePt.GetLatitude().GetDegrees(),
                                          surfacePt.GetLongitude().GetDegrees());
      }

      double radius = 0.;
      if (success) {
        portal->SetPosition(ugm->Sample(), ugm->Line(), 1);
        demCube.read(*portal);
        radius = interp->Interpolate(ugm->Sample(), ugm->Line(),
                                     portal->DoubleBuffer());
        success = !IsSpecial(radius);
      }

      // if we are unable to calculate a valid Radius value for a point,
      // we will ignore this point and keep track of it
      if (!success) {
        numFailures++;
        if (numFailures > 1) {
          failedIDs = failedIDs + ", ";
        }
        failedIDs = failedIDs + cp->GetId();
        cp->SetIgnored(true);
      }
      // otherwise, we will replace the computed radius value to the output control net
      else {
        numSuccesses++;
        surfacePt.ResetLocalRadius(Distance(radius, Distance::Meters));
        if (newRadiiSource == Adjusted) {
          cp->SetAdjustedSurfacePoint(surfacePt);
        }
        else if (newRadiiSource == Apriori) {
          cp->SetAprioriSurfacePoint(surfacePt);
          // Update radius source and radius source file
          cp->SetAprioriRadiusSource(ControlPoint::RadiusSource::DEM);
          cp->SetAprioriRadiusSourceFile(demFile);
        }
      }
    }
  }

  delete ugm;
  ugm = NULL;
  delete interp;
  interp = NULL;
  delete portal;
  portal = NULL;

  if (numSuccesses == 0) {
    if (numConstrainedFixed == 0) {
      string msg = "There were no Fixed or Constrained points in this network."
          "  No radii were replaced.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
    else {
      string msg = "No valid radii can be calculated. Verify that the DEM [" +
                       ui.GetAsString("MODEL") + "] is valid.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
  }

  cnet.Write(ui.GetFilename("ONET"));

  // Write results to Logs
  // Summary group is created with the counts of successes and failures
  PvlGroup summaryGroup = PvlGroup("Summary");
  summaryGroup.AddKeyword(PvlKeyword("Successes", numSuccesses));
  summaryGroup.AddKeyword(PvlKeyword("Failures", numFailures));
  summaryGroup.AddKeyword(PvlKeyword("NumberFixedConstrainedPoints",
                                      numConstrainedFixed));

  bool errorlog;
  Filename errorlogFile;
  // if a filename was entered, use it to create the log
  if (ui.WasEntered("ERRORS")) {
    errorlog = true;
    errorlogFile = ui.GetFilename("ERRORS");
  }
  // if no filename was entered, but there were some failures,
  // create an error log named "failures" in the current directory
  else if (numFailures > 0) {
    errorlog = true;
    errorlogFile = "failures.log";
  }
  // if all radii are successfully calculated and no error
  // file is named, only output summary to application log
  else {
    errorlog = false;
  }

  if (errorlog) {
    // Write details in error log
    Pvl results;
    results.SetName("Results");
    results.AddGroup(summaryGroup);
    if (numFailures > 0) {
      // if there are any failures, add comment to the summary log to alert user
      summaryGroup.AddComment("Unable to calculate radius for all points. Point"
              " IDs for failures contained in [" + errorlogFile.Name() + "].");
      PvlGroup failGroup = PvlGroup("Failures");
      failGroup.AddComment("A point fails if we are unable to set universal "
               "ground or if the radius calculated is a special pixel value.");
      failGroup.AddKeyword(PvlKeyword("PointIDs", failedIDs));
      results.AddGroup(failGroup);
    }
    results.Write(errorlogFile.Expanded());
  }
  // Write summary to application log
  Application::Log(summaryGroup);
}
