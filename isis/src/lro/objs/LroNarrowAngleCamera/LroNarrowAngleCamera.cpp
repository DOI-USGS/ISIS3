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
#include "LroNarrowAngleCamera.h"

#include <iomanip>

#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "LroNarrowAngleDistortionMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for the LRO NAC Camera Model
   *
   * @param lab Pvl Label to create the camera model from
   *
   * @internal 
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  LroNarrowAngleCamera::LroNarrowAngleCamera(Pvl &lab) : LineScanCamera(lab) {
    NaifStatus::CheckErrors();
    // Set up the camera info from ik/iak kernels
    SetFocalLength();
    SetPixelPitch();

    double constantTimeOffset = 0.0,
           additionalPreroll = 0.0,
           additiveLineTimeError = 0.0,
           multiplicativeLineTimeError = 0.0;

    QString ikernKey = "INS" + toString(naifIkCode()) + "_CONSTANT_TIME_OFFSET";
    constantTimeOffset = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode()) + "_ADDITIONAL_PREROLL";
    additionalPreroll = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode()) + "_ADDITIVE_LINE_ERROR";
    additiveLineTimeError = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode()) + "_MULTIPLI_LINE_ERROR";
    multiplicativeLineTimeError = getDouble(ikernKey);

    // Get the start time from labels
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockPrerollCount"];
    SpiceDouble etStart;

    if(stime != "NULL") {
      etStart = getClockTime(stime).Et();
    }
    else {
      etStart = iTime((QString)inst["PrerollTime"]).Et();
    }

    // Get other info from labels
    double csum = inst["SpatialSumming"];
    double lineRate = (double) inst["LineExposureDuration"] / 1000.0;
    double ss = inst["SampleFirstPixel"];
    ss += 1.0;

    lineRate *= 1.0 + multiplicativeLineTimeError;
    lineRate += additiveLineTimeError;
    etStart += additionalPreroll * lineRate;
    etStart += constantTimeOffset;

    setTime(etStart);

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap = new LineScanCameraDetectorMap(this, etStart, lineRate);
    detectorMap->SetDetectorSampleSumming(csum);
    detectorMap->SetStartingDetectorSample(ss);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    //  Retrieve boresight location from instrument kernel (IK) (addendum?)
    ikernKey = "INS" + toString(naifIkCode()) + "_BORESIGHT_SAMPLE";
    double sampleBoreSight = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode()) + "_BORESIGHT_LINE";
    double lineBoreSight = getDouble(ikernKey);

    focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
    focalMap->SetDetectorOffset(0.0, 0.0);

    // Setup distortion map
    LroNarrowAngleDistortionMap *distMap = new LroNarrowAngleDistortionMap(this);
    distMap->SetDistortion(naifIkCode());

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}

/**
 * This is the function that is called in order to instantiate a
 * LroNarrowAngle object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* LroNarrowAngleCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Lro namespace.
 */
extern "C" Isis::Camera *LroNarrowAngleCameraPlugin(Isis::Pvl &lab) {
  return new Isis::LroNarrowAngleCamera(lab);
}
