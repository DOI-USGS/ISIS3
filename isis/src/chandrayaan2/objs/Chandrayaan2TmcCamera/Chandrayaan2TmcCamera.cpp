/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Chandrayaan2TmcCamera.h"

#include <QString>

#include "Affine.h"
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
    //Chandrayaan2 iak uses INS-152???_PIXEL_SIZE instead of PIXEL_PITCH
    QString ikernKey = "INS" + toString(naifIkCode()) + "_PIXEL_SIZE";
    // Pixel size is specified in meters, we need it in mm
    SetPixelPitch(getDouble(ikernKey)*1000);

    // Get the start time from labels
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = (QString)inst["StartTime"];
    SpiceDouble etStart=0;
    etStart = iTime(stime).Et();

    double csum = 1;
    if (inst.hasKeyword("SpatialSumming")){
      csum = inst["SpatialSumming"];
    }
    double lineRate = (double) inst["LineExposureDuration"] / 1000;

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap =
      new LineScanCameraDetectorMap(this, etStart, lineRate);
    detectorMap->SetDetectorSampleSumming(csum);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    //  Retrieve boresight location from instrument kernel (IK) (addendum?)
    // Shift sample by 0.5 to align ISIS with the CSM/ALE convention.
    QString centerKey = "INS" + toString((int)naifIkCode()) + "_CENTER";
    double sampleCenter = getDouble(centerKey, 0) + 0.5;
    double lineCenter = getDouble(centerKey, 1);

    focalMap->SetDetectorOrigin(sampleCenter, lineCenter);
    focalMap->SetDetectorOffset(0.0, 0.0);

    // @TODO set no distortion? No values in IK
    // Setup distortion map
    CameraDistortionMap *distMap = new CameraDistortionMap(this);
    distMap->SetDistortion(naifIkCode());

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    // Set proper end frame
    int tmcFrame(0);
    if (naifIkCode() == -152210) {
      tmcFrame = -152220;
    }
    else if (naifIkCode() == -152211) {
      tmcFrame = -152221;
    }
    else if (naifIkCode() == -152212) {
      tmcFrame = -152222;
    }
    else {
      QString msg = "Unknown frame code [" + toString(naifIkCode()) + "] for Chandrayaan2 TMC Camera.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    instrumentRotation()->SetFrame(tmcFrame);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}


/**
 * This is the function that is called in order to instantiate an Chandrayaan2TmcCamera object.
 *
 * @param cube The input cube from which to instantiate the camera model.
 *
 * @return Isis::Camera* Chandrayaan2TmcCamera
 *
 */
extern "C" Isis::Camera *Chandrayaan2TmcCameraPlugin(Isis::Cube &cube) {
  return new Isis::Chandrayaan2TmcCamera(cube);
}
