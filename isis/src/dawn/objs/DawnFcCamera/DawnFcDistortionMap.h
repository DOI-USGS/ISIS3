#ifndef DawnFcDistortionMap_h
#define DawnFcDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class DawnFcDistortionMap : public CameraDistortionMap {
    public:
      DawnFcDistortionMap(Camera *parent, double k1, double zDirection = 1.0);
      ~DawnFcDistortionMap() {};

      bool SetFocalPlane(const double ux, const double uy);
      bool SetUndistortedFocalPlane(const double dx, const double dy);

    private:
      double p_k1;
  };
};
#endif
