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

#include "ClipperWacFcCamera.h"

#include <QString>

#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {

/**
   * Constructs a Clipper wide angle framing camera object.
   *
   * @param lab Pvl label from a Clipper wide angle framing camera image.
   *
   * @author Stuart Sides and Summer Stapleton-Greig 
   *  
   * @internal
   */
  ClipperWacFcCamera::ClipperWacFcCamera(Cube &cube) : FramingCamera(cube) {
    m_spacecraftNameLong = "Europa Clipper";
    m_spacecraftNameShort = "Clipper";

    m_instrumentNameLong  = "Europa Imaging System Framing Wide Angle Camera";
    m_instrumentNameShort = "EIS-FWAC";

    NaifStatus::CheckErrors();

    SetFocalLength(); 
    SetPixelPitch();

    // Set up detector map, focal plane map, and distortion map
    new CameraDetectorMap(this);
    new CameraFocalPlaneMap(this, naifIkCode());
    new CameraDistortionMap(this);
    
    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString startTime = inst["StartTime"];
    iTime etStart(startTime);

    // double exposureDuration = (double)inst["ExposureDuration"] / 1000.0;
    // pair<iTime, iTime> startStop = ShutterOpenCloseTimes(et, exposureDuration);

     setTime(etStart.Et()); // Set the time explicitly for now to prevent segfault

    // Internalize all the NAIF SPICE information into memory.
    LoadCache();
    NaifStatus::CheckErrors();
  }

  /**
   * Returns the shutter open and close times.  The LORRI camera doesn't use a shutter to start and 
   * end an observation, but this function is being used to get the observation start and end times,
   * so we will simulate a shutter. 
   * 
   * @param exposureDuration ExposureDuration keyword value from the labels,
   *                         converted to seconds.
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   */
  pair<iTime, iTime> ClipperWacFcCamera::ShutterOpenCloseTimes(double time,
                                                         double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time - exposureDuration / 2.0, exposureDuration);
  }
}

/**
 * This is the function that is called in order to instantiate a ClipperWacFcCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* ClipperWacFcCamera
 * @internal
 */
extern "C" Isis::Camera *ClipperWacFcCameraPlugin(Isis::Cube &cube) {
  return new Isis::ClipperWacFcCamera(cube);
}
