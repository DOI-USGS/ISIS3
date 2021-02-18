#ifndef JunoDistortionMap_h
#define JunoDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates for Juno's JunoCam camera.
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2017-08-04 Jeannie Backer and Kristin Berry
   *
   * @internal
   *   @history 2017-08-04 Jeannie Backer and  Kristin Berry - Original version.
   *
   */
  class JunoDistortionMap : public CameraDistortionMap {
    public:
      JunoDistortionMap(Camera *parent);

      virtual void SetDistortion(int naifIkCode);

      virtual ~JunoDistortionMap();

      virtual bool SetFocalPlane(double dx, double dy);

      virtual bool SetUndistortedFocalPlane(double ux, double uy);
  };
};
#endif
