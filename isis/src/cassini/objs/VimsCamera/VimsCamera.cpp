/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "VimsCamera.h"
#include "VimsGroundMap.h"
#include "VimsSkyMap.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Constants.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
  /**
   * Constructor for the Cassini Vims Camera Model
   *
   * @param [in] lab   (Pvl &)  Label used to create camera model
   *
   * @internal 
   *   @history 2007-12-12  Tracie Sucharski,  After creating spice cache with
   *                           padding, reset et by calling SetImage(1,1) so that
   *                           et is initialized properly at beginning of image
   *                           without padding.
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   *
   */
  VimsCamera::VimsCamera(Pvl &lab) : Camera(lab) {
    NaifStatus::CheckErrors();

    PvlGroup inst = lab.FindGroup("Instrument", Pvl::Traverse);
    string channel = (string) inst ["Channel"];

    // Set Frame mounting

    if(channel == "VIS") {
      //LoadFrameMounting ("CASSINI_SC_COORD","CASSINI_VIMS_V");

      SetFocalLength(143.0);
      if(iString((string)inst["SamplingMode"]).UpCase() == "NORMAL") {
        SetPixelPitch(3 * .024);
      }
      else {
        SetPixelPitch(.024);
      }
    }
    else if(channel == "IR") {
      //LoadFrameMounting ("CASSINI_SC_COORD","CASSINI_VIMS_IR");

      SetFocalLength(426.0);
      SetPixelPitch(.2);
    }

    // Get the start time in et
    iString stime = (string) inst ["NativeStartTime"];
    string intTime = stime.Token(".");

    double etStart = getClockTime(intTime).Et();

    //  Add 2 seconds to either side of time range because the time are for IR
    // channel, the VIS may actually start integrating before NATIVE_START_TIME.
    //  This insures the cache is large enough.
    etStart += stime.ToDouble() / 15959.0 - 2.;

    // Get the end time in et
    iString etime = (string) inst ["NativeStopTime"];
    intTime = etime.Token(".");

    double etStop = getClockTime(intTime).Et();

    //  Add 2 seconds to either side of time range because the time are for IR
    // channel, the VIS may actually start integrating before NATIVE_START_TIME.
    //  This insures the cache is large enough.
    etStop += stime.ToDouble() / 15959.0 + 2.;

    //  Setup detector map
    new CameraDetectorMap(this);

    // Setup focal plane map
    new CameraFocalPlaneMap(this, NaifIkCode());

    // Setup distortion map
    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new VimsGroundMap(this, lab);
    new VimsSkyMap(this, lab);

    ((VimsGroundMap *)GroundMap())->Init(lab);
    ((VimsSkyMap *)SkyMap())->Init(lab);

    double tol = PixelResolution();

    if(tol < 0.) {
      // Alternative calculation of .01*ground resolution of a pixel
      tol = PixelPitch() * SpacecraftAltitude() / FocalLength() / 1000. / 100.;
    }

    if(channel == "VIS") CreateCache(etStart, etStop, 64 * 64, tol);
    if(channel == "IR") CreateCache(etStart, etStop, 64 * 64, tol);

    //  Call SetImage so that the et is reset to beginning of image w/o
    //   padding.
    IgnoreProjection(true);
    SetImage(1, 1);
    IgnoreProjection(false);
    NaifStatus::CheckErrors();
    return;
  }
}


// Plugin
/** 
 * This is the function that is called in order to instantiate a VimsCamera object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* VimsCamera
 * @internal 
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed
 *            Cassini namespace.
 */
extern "C" Isis::Camera *VimsCameraPlugin(Isis::Pvl &lab) {
  return new Isis::VimsCamera(lab);
}

