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
#include "NirCamera.h"

#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "RadialDistortionMap.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a Clementine HiresCamera object using the image labels.
   *
   * @param lab Pvl label from a Clementine HIRES image.
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.  Added
   *                          call to ShutterOpenCloseTimes() method. Changed
   *                          centertime to add half exposure duration to start
   *                          time to maintain consistency with other Clementine
   *                          models. 
   */
  NirCamera::NirCamera(Pvl &lab) : FramingCamera(lab) {
    NaifStatus::CheckErrors();
    // Get the camera characteristics
    QString filter = (QString)(lab.FindGroup("BandBin", Pvl::Traverse))["FilterName"];
    filter = filter.toUpper();

    if(filter.compare("A") == 0) {
      SetFocalLength(2548.2642 * 0.038);
    }
    else if(filter.compare("B") == 0) {
      SetFocalLength(2530.8958 * 0.038);
    }
    else if(filter.compare("C") == 0) {
      SetFocalLength(2512.6589 * 0.038);
    }
    else if(filter.compare("D") == 0) {
      SetFocalLength(2509.0536 * 0.038);
    }
    else if(filter.compare("E") == 0) {
      SetFocalLength(2490.7378 * 0.038);
    }
    else if(filter.compare("F") == 0) {
      SetFocalLength(2487.8694 * 0.038);
    }

    SetPixelPitch();

    // Get the start time in et
    PvlGroup inst = lab.FindGroup("Instrument", Pvl::Traverse);

    // set variables startTime and exposureDuration
    double et = iTime((QString)inst["StartTime"]).Et();

    // divide exposure duration keyword value by 1000 to convert to seconds
    double exposureDuration = ((double) inst["ExposureDuration"]) / 1000.0;
    pair<iTime, iTime> shuttertimes = ShutterOpenCloseTimes(et, exposureDuration);

    /************************************************************************
     * The following line was uncommented to maintain consistency within all
     * clementine camera models. Not sure why the following was originally
     * commented out:
     * 2010-08-05 Jeannie Walldren
     ***********************************************************************/ 
    // Do not correct time for center of the exposure duration. This is because
    // the kernels were built to accept the start times of the images.
    iTime centerTime = shuttertimes.first.Et() + exposureDuration / 2.0;

    // Setup detector map
    new CameraDetectorMap(this);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    focalMap->SetDetectorOrigin(
      Spice::getDouble("INS" + toString(naifIkCode()) + 
                       "_BORESIGHT_SAMPLE"),
      Spice::getDouble("INS" + toString(naifIkCode()) + 
                       "_BORESIGHT_LINE"));

    // Setup distortion map
    new RadialDistortionMap(this, -0.0006364);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    setTime(centerTime);
    LoadCache();
    NaifStatus::CheckErrors();
  }

  /**
   * Returns the shutter open and close times. The user should pass in the
   * ExposureDuration keyword value, converted from milliseconds to seconds, and
   * the StartTime keyword value, converted to ephemeris time. The StartTime
   * keyword value from the labels represents the time at the start of the
   * observation, as noted in the Clementine EDR image SIS. This method uses the
   * FramingCamera class implementation, returning the given time value as the
   * shutter open and the sum of the time value and exposure duration as the
   * shutter close.
   *
   * @param exposureDuration ExposureDuration keyword value from the labels,
   *                         converted to seconds.
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @see http://pds-imaging.jpl.nasa.gov/documentation/clementine_edrsis.pdf
   * @author 2011-05-03 Jeannie Walldren
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Original version.
   */
  pair<iTime, iTime> NirCamera::ShutterOpenCloseTimes(double time,
                                                      double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }
}

/**
 * This is the function that is called in order to instantiate a NirCamera
 * object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* NirCamera
 * @internal 
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed
 *            Clementine namespace.
 */
extern "C" Isis::Camera *NirCameraPlugin(Isis::Pvl &lab) {
  return new Isis::NirCamera(lab);
}
