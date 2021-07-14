#ifndef Hyb2OncDistortionMap_h
#define Hyb2OncDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraDistortionMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates for Hayabusa 2's ONC cameras.
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2017-07-11 Jeannie Backer and Ian Humphrey
   *
   * @internal
   *   @history 2017-07-11 Jeannie Backer and Ian Humphrey - Original version.
   *
   */
  class Hyb2OncDistortionMap : public CameraDistortionMap {
    public:
      Hyb2OncDistortionMap(Camera *parent, double zDirection = 1.0);

      virtual ~Hyb2OncDistortionMap();

      virtual bool SetFocalPlane(double dx, double dy);

      virtual bool SetUndistortedFocalPlane(double ux, double uy);
  };
};
#endif
