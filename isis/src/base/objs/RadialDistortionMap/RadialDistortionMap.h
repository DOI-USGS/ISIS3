#ifndef NirDistortionMap_h
#define NirDistortionMap_h
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
  class RadialDistortionMap : public CameraDistortionMap {
    public:
      RadialDistortionMap(Camera *parent, double k1, double zDirection = 1.0);
      ~RadialDistortionMap() {};

      bool SetFocalPlane(const double dx, const double dy);
      bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double p_k1;
      //double p_cameraSpec;
  };
};
#endif
