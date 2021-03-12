#ifndef ClipperNacRollingShutterCamera_h
#define ClipperNacRollingShutterCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

 #include "RollingShutterCamera.h"

namespace Isis {
  class Cube;

  /**
   * @brief Clipper EIS Camera model
   *
   * This is the camera model for the Clipper EIS NAC Rolling Shutter Camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Clipper
   *
   * @author 2018-04-09 Makayla Shepherd and Ian Humphrey
   *
   *
   */
  class ClipperNacRollingShutterCamera : public RollingShutterCamera {
    public:
      ClipperNacRollingShutterCamera(Cube &cube);

      ~ClipperNacRollingShutterCamera();

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};

#endif
