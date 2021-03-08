/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
  LroNarrowAngleCamera::LroNarrowAngleCamera(Cube &cube) : LineScanCamera(cube) {
    m_spacecraftNameLong = "Lunar Reconnaissance Orbiter";
    m_spacecraftNameShort = "LRO";
    // NACL instrument kernel code = -85600
    if (naifIkCode() == -85600) {
      m_instrumentNameLong = "Narrow Angle Camera Left";
      m_instrumentNameShort = "NACL";
    }
    // NACR instrument kernel code = -85610
    else if (naifIkCode() == -85610) {
      m_instrumentNameLong = "Narrow Angle Camera Right";
      m_instrumentNameShort = "NACR";
    }
    else {
      QString msg = "File does not appear to be a Lunar Reconnaissance Orbiter Image: ";
      msg += QString::number(naifIkCode());
      msg += " is not a supported instrument kernel code for Lunar Reconnaissance Orbiter.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
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
    Pvl &lab = *cube.label();
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
extern "C" Isis::Camera *LroNarrowAngleCameraPlugin(Isis::Cube &cube) {
  return new Isis::LroNarrowAngleCamera(cube);
}
