#ifndef ShadowCamCamera_h
#define ShadowCamCamera_h
/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

#include "Cube.h"
#include "LineScanCamera.h"

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
      /**
       * Constructor for the ShadowCam Camera Model
       *
       * @param cube The cube for which to construct a KPLO ShadowCam Camera object.
       *
       * @internal
       *   @history 2022-10-12 Victor Silva - original object
       *   @history 2024-07-12 Victor Silva - updated to use SpacecraftClockPrerollCount
       *                                      and use getClockTime to use SCLKS
       *   @history 2025-09-08 Victor Silva - updated to use SpacecraftStartTime + StartTimeOffset
       *                                      from labels instead of deriving in kernel.
       */
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
      virtual int CkFrameId() const { return -155151; }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix" Kernel Reference ID
       */
      virtual int CkReferenceId() const { return 1; }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return 1; }

    private:
  };
}

#endif
