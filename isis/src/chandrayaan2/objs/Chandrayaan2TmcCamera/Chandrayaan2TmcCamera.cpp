/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Chandrayaan2TmcCamera.h"

#include <QString>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "iTime.h"
#include "IString.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Chandrayaan 2 TMC Camera object using the image labels.
   *
   */
  Chandrayaan2TmcCamera::Chandrayaan2TmcCamera(Cube &cube) : LineScanCamera(cube) {
    m_instrumentNameLong = "Terrain Mapping Camera-2";
    m_instrumentNameShort = "TMC-2";
    m_spacecraftNameLong = "Chandrayaan 2";
    m_spacecraftNameShort = "Chan2";

    NaifStatus::CheckErrors();
    // Set up the camera info from ik/iak kernels
    SetFocalLength();
    SetPixelPitch();

    // Get the start time from labels
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockStartCount"];
    double etStart = getClockTime(stime).Et();

    // Get other info from labels
    double csum = inst["SpatialSumming"];
    double lineRate = (double) inst["LineExposureDuration"] / 1000.0;

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap =
      new LineScanCameraDetectorMap(this, etStart, lineRate);
    detectorMap->SetDetectorSampleSumming(csum);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    //  Retrieve boresight location from instrument kernel (IK) (addendum?)
    QString boresightkey = "INS" + toString((int)naifIkCode()) + "_BORESIGHT";
    double sampleBoreSight = getDouble(boresightkey, 0);
    double lineBoreSight = getDouble(boresightkey, 1);

    focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
    focalMap->SetDetectorOffset(0.0, 0.0);

    // Setup distortion map
    CameraDistortionMap *distMap = new CameraDistortionMap(this);
    distMap->SetDistortion(naifIkCode());

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}


/**
 * This is the function that is called in order to instantiate an Chandrayaan2TmcCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* Chandrayaan2TmcCamera
 *
 */
extern "C" Isis::Camera *Chandrayaan2TmcCameraPlugin(Isis::Cube &cube) {
  return new Isis::Chandrayaan2TmcCamera(cube);
}
