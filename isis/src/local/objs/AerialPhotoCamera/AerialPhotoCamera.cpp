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
#include "AerialPhotoCamera.h"

#include "Affine.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"

#include <QDebug>

using namespace std;
namespace Isis {
  /**
   * Constructs a generic aerial photo camera object using the image labels.
   *
   * @param cube An aerial photo image.
   *
   * @internal
   *   @history 2014-01-14 Jeff Anderson - Original version
   */
  AerialPhotoCamera::AerialPhotoCamera(Cube &cube) : FramingCamera(cube) {
    NaifStatus::CheckErrors();

    // Prep for getting information from cube labels
    Pvl &lab = *cube.label();
    const PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);

    // Set camera focal length
    double focalLength = inst["FocalLength"];
    SetFocalLength(focalLength);

    // Get the fidicual locations which will be needed for the focal plane map
    double fiducialX[8], fiducialY[8], fiducialS[8], fiducialL[8];
    for (int i=0; i<8; i++) {
      fiducialX[i] = inst["FiducialX"][i].toDouble();
      fiducialY[i] = inst["FiducialY"][i].toDouble();
      fiducialS[i] = inst["FiducialSample"][i].toDouble();
      fiducialL[i] = inst["FiducialLine"][i].toDouble();
    }

    // Get the min/max range of the fiducial locations so we can estimate the pixel pitch
    double minX = fiducialX[0];
    double maxX = fiducialX[0];
    double minY = fiducialY[0];
    double maxY = fiducialY[0];

    double minS = fiducialS[0];
    double maxS = fiducialS[0];
    double minL = fiducialL[0];
    double maxL = fiducialL[0];

    for (int i=1; i<8; i++) {
      if (fiducialX[i] < minX) minX = fiducialX[i];
      if (fiducialX[i] > maxX) maxX = fiducialX[i];
      if (fiducialY[i] < minY) minY = fiducialY[i];
      if (fiducialY[i] > maxY) maxY = fiducialY[i];

      if (fiducialS[i] < minS) minS = fiducialS[i];
      if (fiducialS[i] > maxS) maxS = fiducialS[i];
      if (fiducialL[i] < minL) minL = fiducialL[i];
      if (fiducialL[i] > maxL) maxL = fiducialL[i];
    }

    double XYrange = (maxX - minX) > (maxY - minY) ? maxX - minX : maxY - minY;
    double pixelRange = (maxS - minS) > (maxL - minL) ? maxS - minS : maxL - minL;

    double pixelPitch = XYrange / pixelRange;
    SetPixelPitch(pixelPitch); // Just the estimated pixel pitch.  Not perfect but should work in
                               // cam2map when computing pixel resolution for output map.


    // Setup detector map.  There is no summing or starting sample/line so this map should be the
    // identity (image sample/line == detector sample/line)
    new CameraDetectorMap(this);

    // Setup focal plane map.  This maps image line/samples to focal plane mm using the fiducials
    Affine affine;
    affine.Solve(fiducialS, fiducialL, fiducialX, fiducialY, 8);
    new CameraFocalPlaneMap(this, affine);

    // Setup the optical distortion model.  Use the Apollo metric distortion model which allows for decentering.
#if 0
    double xCalPP = inst["XCalibratedPrinicpalPoint"][0].toDouble();
    double yCalPP = inst["XCalibratedPrinicpalPoint"][0].toDouble();

    double odk0 = inst["RadialDistortionCoefficients"][0].toDouble();
    double odk1 = inst["RadialDistortionCoefficients"][1].toDouble();
    double odk2 = inst["RadialDistortionCoefficients"][2].toDouble();

    double dc0 = inst["DecenteringDistortionCoefficients"][0].toDouble();
    double dc1 = inst["DecenteringDistortionCoefficients"][1].toDouble();
    double dc2 = inst["DecenteringDistortionCoefficients"][2].toDouble();

    new ApolloMetricDistortionMap(this, xCalpp, yCalpp, odk0, odk1, odk2, dc0, dc1, dc2);
#endif
    new CameraDistortionMap(this, -1.0);
//    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    // Create a cache and grab spice info since it does not change for
    // a framing camera (fixed spacecraft position and pointing)
    setTime(inst["EphemerisTime"][0].toDouble());
    LoadCache();
    NaifStatus::CheckErrors();
  }

  /**
   * Returns the shutter open and close times. The user should pass in the
   * exposure duration in seconds and the StartTime keyword value, converted to
   * ephemeris time. The StartTime keyword value from the labels represents the
   * shutter center time of the observation. To find the shutter open and close
   * times, half of the exposure duration is subtracted from and added to the
   * input time parameter, respectively.  This method overrides the
   * FramingCamera class method.
   * @b Note: Apollo did not provide exposure duration in the support data.
   *
   * @param exposureDuration Exposure duration, in seconds.
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @author 2011-05-03 Jeannie Walldren
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Original version.
   */
  pair<iTime, iTime> AerialPhotoCamera::ShutterOpenCloseTimes(double time,
                                                               double exposureDuration) {
    pair<iTime, iTime> shuttertimes;
    // To get shutter start (open) time, subtract half exposure duration
    shuttertimes.first = time - (exposureDuration / 2.0);
    // To get shutter end (close) time, add half exposure duration
    shuttertimes.second = time + (exposureDuration / 2.0);
    return shuttertimes;
  }
} 

/**
 * This is the function that is called in order to instantiate an
 * AerialPhotoCamera object.
 *
 * @param lab Cube labels
 *
 * @return Camera* AerialPhotoCamera
 * @internal
 */
extern "C" Isis::Camera *AerialPhotoCameraPlugin(Isis::Cube &cube) {
  return new Isis::AerialPhotoCamera(cube);
}
