#ifndef ShadowCamCamera_h
#define ShadowCamCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"
#include "NaifStatus.h"  // Add missing header if needed for NaifStatus

namespace Isis {
  /**
   * @brief KPLO ShadowCam Camera Model
   *
   * This is the camera model for the Korea Pathfinder Lunar Orbiter ShadowCam
   * camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup KoreaPathfinderLunarOrbiter
   *
   * @author 2022-10-12 Victor Silva
   *
   * @internal
   *   @history 2022-10-12 Victor Silva, Original Object
   */

  class ShadowCamCamera : public LineScanCamera {
    public:
      ShadowCamCamera(Cube &cube);
      ~ShadowCamCamera() {};

      /**
       * @brief Retrieves a double value from the IK based on the naifIkCode and the provided keyword.
       *
       * @param naifCode The NAIF instrument kernel code (e.g., -155151).
       * @param keyword The keyword to retrieve the value for.
       *
       * @return double The retrieved value.
       */
      double getIkernValue(int naifCode, const QString &keyword);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-155151); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      // Declare any additional member variables if necessary
  };
};
#endif
