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

#include "LwirCamera.h"

#include <QString>

#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifContext.h"
#include "RadialDistortionMap.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a LwirCamera object using the image labels
   *
   * @param lab Pvl label from a Clementine LWIR image.
   *
   * @internal
   *   @history 2009-01-16 Jeannie Walldren - Original Version
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.  Added
   *                          call to ShutterOpenCloseTimes() method. Changed
   *                          centertime to add half exposure duration to start
   *                          time to maintain consistency with other Clementine
   *                          models.
   */
  LwirCamera::LwirCamera(Cube &cube) : FramingCamera(cube) {
    auto naif = NaifContext::acquire();

    m_instrumentNameLong = "Long Wave Infrared Camera";
    m_instrumentNameShort = "LWIR";
    m_spacecraftNameLong = "Clementine 1";
    m_spacecraftNameShort = "Clementine1";

    naif->CheckErrors();

    // Get the camera characteristics
    Pvl &lab = *cube.label();
    QString filter = (lab.findGroup("BandBin", Pvl::Traverse))["FilterName"];

    filter = filter.toUpper();

    SetFocalLength(naif);
    SetPixelPitch(naif);

    // Get the start time in et
    PvlGroup inst = lab.findGroup("Instrument", Pvl::Traverse);

    // set variables startTime and exposureDuration
    double time = iTime((QString)inst["StartTime"]).Et();

    // divide exposure duration keyword value by 1000 to convert to seconds
    double exposureDuration = ((double) inst["ExposureDuration"]) / 1000.0;
    pair<iTime, iTime> shuttertimes = ShutterOpenCloseTimes(time, exposureDuration);

    /************************************************************************
     * The following line was uncommented to maintain consistency within all
     * clementine camera models. Not sure why the following was originally
     * commented out:
     * 2010-08-05 Jeannie Walldren
     ***********************************************************************/
    // Do not correct time for center of the exposure duration. This is because
    // the kernels were built to accept the start times of the images.
    iTime centerTime = shuttertimes.first.Et() + exposureDuration / 2.0; // Add half exposure duration

    // Setup detector map
    new CameraDetectorMap(this);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    focalMap->SetDetectorOrigin(
      Spice::getDouble(naif, "INS" + toString(naifIkCode()) +
                       "_BORESIGHT_SAMPLE"),
      Spice::getDouble(naif, "INS" + toString(naifIkCode()) +
                       "_BORESIGHT_LINE"));

    // Setup distortion map
    new RadialDistortionMap(this, 0.0);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    setTime(centerTime, naif);
    LoadCache(naif);
    naif->CheckErrors();
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
  pair<iTime, iTime> LwirCamera::ShutterOpenCloseTimes(double time,
                                                       double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }
}


/**
 * This is the function that is called in order to instantiate a LwirCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* LwirCamera
 * @internal
 *   @history 2009-01-16 Jeannie Walldren - Original Version
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed
 *            Clementine namespace.
 */
extern "C" Isis::Camera *LwirCameraPlugin(Isis::Cube &cube) {
  return new Isis::LwirCamera(cube);
}
