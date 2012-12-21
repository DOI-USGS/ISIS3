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
// $Id: IssWACamera.cpp,v 1.6 2009/08/31 15:12:29 slambright Exp $
#include "IssWACamera.h"

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
   * Constructs a IssWACamera object using the image labels.
   *
   * @param lab Pvl label from a Cassini ISS Wide Angle Camera image.
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check. Added call
   *                          to ShutterOpenCloseTimes() method.
   */
  IssWACamera::IssWACamera(Pvl &lab) : FramingCamera(lab) {
    NaifStatus::CheckErrors();
    PvlGroup bandBin = lab.FindGroup("BandBin", Pvl::Traverse);
    // Get the camera characteristics
    QString key = "INS" + toString(naifIkCode()) + "_" + bandBin["FilterName"][0] + "_FOCAL_LENGTH";
    key = key.replace("/", "_");
    double focalLength = Spice::getDouble(key);

    SetFocalLength(focalLength);
    SetPixelPitch();
    instrumentRotation()->SetFrame(Spice::getInteger("INS_" + toString(naifIkCode()) + "_FRAME_ID"));


    // Get the start time in et
    PvlGroup inst = lab.FindGroup("Instrument", Pvl::Traverse);

    double et = iTime((QString)inst["StartTime"]).Et();

    // divide exposure duration keyword value by 1000 to convert to seconds
    double exposureDuration = ((double) inst["ExposureDuration"]) / 1000.0;
    pair<iTime, iTime> shuttertimes = ShutterOpenCloseTimes(et, exposureDuration);

    //correct time for center of exposure duration
    iTime centerTime = shuttertimes.first.Et() + exposureDuration / 2.0;

    // Setup detector map
    int summingMode = inst["SummingMode"];
    CameraDetectorMap *detectorMap = new CameraDetectorMap(this);
    detectorMap->SetDetectorLineSumming(summingMode);
    detectorMap->SetDetectorSampleSumming(summingMode);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    focalMap->SetDetectorOrigin(Spice::getDouble("INS" + toString(naifIkCode()) + "_BORESIGHT_SAMPLE"),
                                Spice::getDouble("INS" + toString(naifIkCode()) + "_BORESIGHT_LINE"));

    // Setup distortion map
    double k1 = Spice::getDouble("INS" + toString(naifIkCode()) + "_K1");
    new RadialDistortionMap(this, k1);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    setTime(centerTime);
    LoadCache();
    NaifStatus::CheckErrors();
  }

  /**
   * Returns the shutter open and close times.  The user should pass in the
   * ExposureDuration keyword value, converted from milliseconds to seconds, and
   * the StartTime keyword value, converted to ephemeris time. The StartTime
   * keyword value from the labels represents the shutter open time of the
   * exposure, as noted in the Cassini ISS EDR image SIS. This method uses the
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
   * @see http://pds-imaging.jpl.nasa.gov/documentation/Cassini_edrsis.pdf
   * @author 2011-05-03 Jeannie Walldren
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Original version.
   */
  pair<iTime, iTime> IssWACamera::ShutterOpenCloseTimes(double time,
                                                        double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }
}

/**
 * This is the function that is called in order to instantiate a IssWACamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* IssWACamera
 * @internal 
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed
 *            Cassini namespace.
 */
extern "C" Isis::Camera *IssWACameraPlugin(Isis::Pvl &lab) {
  return new Isis::IssWACamera(lab);
}
