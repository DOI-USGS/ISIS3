/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
// $Id: IssNACamera.cpp,v 1.6 2009/08/31 15:12:29 slambright Exp $
#include "IssNACamera.h"

#include <QString>

#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "RadialDistortionMap.h"
#include "iTime.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a IssNACamera object using the image labels.
   *
   * @param lab Pvl label from a Cassini ISS Narrow Angle Camera image.
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check. Added call
   *                          to ShutterOpenCloseTimes() method.
   */
  IssNACamera::IssNACamera(Cube &cube) : FramingCamera(cube) {
    m_instrumentNameLong = "Imaging Science Subsystem Narrow Angle";
    m_instrumentNameShort = "ISSNA";
    m_spacecraftNameLong = "Cassini Huygens";
    m_spacecraftNameShort = "Cassini";

    Pvl &lab = *cube.label();

    // Get the filter wheels positions dependent focal length.
    // If we can not get the focal length for this specific filter wheel combination then
    // use the default.
    double focalLength = 0.0;
    try {
      PvlGroup bandBin = lab.findGroup("BandBin", Pvl::Traverse);
      QString key = QString("INS%1_%2_FOCAL_LENGTH").
                      arg(naifIkCode()).arg(bandBin["FilterName"][0]);
      key = key.replace("/", "_");
      focalLength = getDouble(key);
    }
    catch (IException &firstException) {
      try {
        QString key = "INS-82360_DEFAULT_FOCAL_LENGTH";
        focalLength = getDouble(key);
      }
      catch (IException &secondException) {
        PvlGroup bandBin = lab.findGroup("BandBin", Pvl::Traverse);
        IException finalError(IException::Unknown,
            QString("Unable to find a focal length for the requested Cassini ISS NA "
                    "filter combination [%1] or the default focal length")
                        .arg(bandBin["FilterName"][0]),
            _FILEINFO_);
        finalError.append(firstException);
        finalError.append(secondException);
        throw finalError;
      }
    }

    NaifStatus::CheckErrors();

    SetFocalLength(focalLength);
    SetPixelPitch();
    instrumentRotation()->SetFrame(Spice::getInteger("INS_" + toString(naifIkCode()) + "_FRAME_ID"));

    // Get the start time in et
    PvlGroup inst = lab.findGroup("Instrument", Pvl::Traverse);

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
  pair<iTime, iTime> IssNACamera::ShutterOpenCloseTimes(double time,
                                                        double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }
}


/**
 * This is the function that is called in order to instantiate a IssNACamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* IssNACamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed
 *            Cassini namespace.
 */
extern "C" Isis::Camera *IssNACameraPlugin(Isis::Cube &cube) {
  return new Isis::IssNACamera(cube);
}
