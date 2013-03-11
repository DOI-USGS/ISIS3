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
#include "MocNarrowAngleCamera.h"

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for the Mgs MOC Narrow Angle Camera Model
   *
   * @param lab Pvl label from an MOC NAC image. 
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  MocNarrowAngleCamera::MocNarrowAngleCamera(Cube &cube) : LineScanCamera(cube) {
    NaifStatus::CheckErrors();
    // Set up the camera info from ik/iak kernels
    //      LoadEulerMounting();
    SetFocalLength();
    SetPixelPitch();
    instrumentRotation()->SetTimeBias(-1.15);

    // Get the start time from labels
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockCount"];
    double etStart = getClockTime(stime).Et();

    // Get other info from labels
    double csum = inst["CrosstrackSumming"];
    double dsum = inst["DowntrackSumming"];
    double lineRate = (double) inst["LineExposureDuration"] / 1000.0;
    lineRate *= dsum;
    double ss = inst["FirstLineSample"];

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap =
      new LineScanCameraDetectorMap(this, etStart, lineRate);
    detectorMap->SetDetectorSampleSumming(csum);
    detectorMap->SetDetectorLineSumming(dsum);
    detectorMap->SetStartingDetectorSample(ss);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap =
      new CameraFocalPlaneMap(this, naifIkCode());
    focalMap->SetDetectorOrigin(1024.5, 0.0);
    focalMap->SetDetectorOffset(0.0, 0.0);

    // Setup distortion map
    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}

/**
 * This is the function that is called in order to instantiate a MocNarrowAngleCamera object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* MocNarrowAngleCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Mgs namespace.
 */
extern "C" Isis::Camera *MocNarrowAngleCameraPlugin(Isis::Cube &cube) {
  return new Isis::MocNarrowAngleCamera(cube);
}
