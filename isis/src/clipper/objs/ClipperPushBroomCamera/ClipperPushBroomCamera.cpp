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

     m_instrumentNameLong  = "Europa Imaging System Push Broom Narrow Angle Camera";
     m_instrumentNameShort = "EIS-PBNAC";

     NaifStatus::CheckErrors();

     SetFocalLength();
     SetPixelPitch();

     Pvl &lab = *cube.label();
     PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
     QString startTime = inst["StartTime"];
     iTime etStart(startTime);

     // TODO: Set up VariableLineScanCameraDetectorMap

     // Set up focal plane map
     CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
     focalMap->SetDetectorOrigin(2048.5, 1024.5);

     // Set up distortion map
     new CameraDistortionMap(this);
     // TODO: set distortion

     // Set up the ground and sky map
     new LineScanCameraGroundMap(this);
     new LineScanCameraSkyMap(this);

     setTime(etStart.Et());
     LoadCache();
     NaifStatus::CheckErrors();
   }

   /**
    * Destructor for a ClipperPushBroomCamera object.
    */
   ClipperPushBroomCamera::~ClipperPushBroomCamera() {
   }

   /**
    * CK frame ID
    *
    * CK frame ID obtained from the CK kernel for Europa Clipper by using spacit.
    * @see Camera::CkFrameId()

    * <tt>spacit -> R -> ck/europa_sa_17F12v2_tour_eom_ecr3018.bc -> INSTRUMENT_ID</tt>

    * @note Could not use spacit's S option, we do not have a SCLK for Clipper EIS yet.
    *
    * @return int The appropriate instrument code for the "Camera-Matrix" Kernel Frame ID.
    */
   int ClipperPushBroomCamera::CkFrameId() const {
     return (-159011);
   }


   /**
    * CK Reference ID - J2000
    *
    * CK reference ID obtained from the CK kernel for Europa Clipper by using spacit.
    * @see Camera::CkReferenceId()
    *
    * <tt>spacit -> R -> ck/europa_sa_17F12v2_tour_eom_ecr3018.bc -> REFERENCE_FRAME_NAME</tt>
    * Look up 'EUROPAM_SA_BASE' in FK kernel for Clipper EIS.
    *
    * @note Could not use spacit's S option, we do not have a SCLK for Clipper EIS yet.
    *
    * @return int The appropriate instrument code for the "Camera-matrix" Kernel Reference ID.
    */
   int ClipperPushBroomCamera::CkReferenceId() const {
     return (-159010);
   }


   /**
    * SPK Reference ID - J2000
    *
    * SPK reference ID obtained from the SPK kernel for Europa Clipper by using spacit.
    * @see Camera::SpkReferenceId()
    *
    * <tt>spacit -> S -> spk/17F12_DIR_L220604_A241223_V2_scpse.bsp, naif0012.tls -> B -> 502</tt>
    * (502 is the NAIF ID for 'EUROPA')
    *
    * @return int The appropriate intstrument code for the Spacecraft Kernel Reference ID.
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
