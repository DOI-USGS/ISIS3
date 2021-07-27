/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ClipperPushBroomCamera.h"

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "iTime.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"
#include "LineScanCameraDetectorMap.h"

namespace Isis {
  /**
   * Constructs a ClipperPushBroomCamera object using the image labels.
   *
   * @param Cube &cube Clipper EIS image.
   */
   ClipperPushBroomCamera::ClipperPushBroomCamera(Cube &cube) : LineScanCamera(cube) {

     m_spacecraftNameLong = "Europa Clipper";
     m_spacecraftNameShort = "Clipper";

     int frameCode = naifIkCode();

     // ik values for NAC: -159101(general), -159110(rad), -159112(koz), filters between -159121 and -159131
     if (frameCode == -159101 || frameCode == -159110 || frameCode == -159112
                              || (frameCode > -159132 && frameCode < -159120)) {
       m_instrumentNameLong  = "Europa Imaging System Push Broom Narrow Angle Camera";
       m_instrumentNameShort = "EIS-PBNAC";
     }
     // ik values for WAC: -159102(general), -159111(rad), -159113(koz), filters between -159141 and -159151
     else if (frameCode == -159102 || frameCode == -159111 || frameCode == -159113
                                   || (frameCode > -159152 && frameCode < -159140)) {
       m_instrumentNameLong  = "Europa Imaging System Push Broom Wide Angle Camera";
       m_instrumentNameShort = "EIS-PBWAC";
     }
     else {
       QString msg = "Unable to construct Clipper Push Broom camera model. "
                     "Unrecognized NaifFrameCode [" + toString(frameCode) + "].";
       throw IException(IException::User, msg, _FILEINFO_);
     }

     NaifStatus::CheckErrors();

     SetFocalLength();
     SetPixelPitch();

     Pvl &lab = *cube.label();
     PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
     double lineRate = ((double) inst["LineExposureDuration"]) / 1000;
     QString startTime = inst["StartTime"];
     iTime etStart(startTime);

     // set up detector map
     new LineScanCameraDetectorMap(this, etStart.Et(), lineRate);

     // Set up focal plane map
     CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
     if (m_instrumentNameShort == "EIS-PBNAC") {
       focalMap->SetDetectorOrigin(2048.5, 1024.5);
     }
     else {
       focalMap->SetDetectorOrigin(2092.5, 1112.5);
     }

     // Set up distortion map
     new CameraDistortionMap(this);
     // TODO: set distortion

     // Set up the ground and sky map
     new LineCameraGroundMap(this);
     new LineScanCameraSkyMap(this);

     LoadCache();
     NaifStatus::CheckErrors();
   }


   /**
    * Destructor for a ClipperPushBroomCamera object.
    */
   ClipperPushBroomCamera::~ClipperPushBroomCamera() {
   }


   /**
    * CK frame ID - Instrument Code from spacit run on CK
    *
    * @return @b int The appropriate instrument code for the "Camera-matrix"
    *         Kernel Frame ID
    */
   int ClipperPushBroomCamera::CkFrameId() const {
     return (-159000);
   }


   /**
    * CK Reference ID - J2000
    *
    * @return @b int The appropriate instrument code for the "Camera-matrix"
    *         Kernel Reference ID
    */
   int ClipperPushBroomCamera::CkReferenceId() const {
     return (1);
   }


   /**
    * SPK Reference ID - J2000
    *
    * @return @b int The appropriate instrument code for the Spacecraft
    *         Kernel Reference ID
    */
   int ClipperPushBroomCamera::SpkReferenceId() const {
     return (1);
   }
}

/**
 * This is the function that is called in order to instantiate an ClipperPushBroomCamera
 * object.
 *
 * @param Isis::Cube &cube Clipper EIS image.
 *
 * @return Isis::Camera* ClipperPushBroomCamera
 */
extern "C" Isis::Camera *ClipperPushBroomCameraPlugin(Isis::Cube &cube) {
  return new Isis::ClipperPushBroomCamera(cube);
}
