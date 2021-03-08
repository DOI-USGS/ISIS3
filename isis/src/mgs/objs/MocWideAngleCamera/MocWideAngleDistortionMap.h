#ifndef MocWideAngleDistortionMap_h
#define MocWideAngleDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  /**
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the Moc wide angle camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsGlobalSurveyor
   *
   * @see Camera
   *
   * @author 2005-02-01 Jeff Anderson
   * @internal
   *   @history 2005-02-01 Jeff Anderson - Original version
   *   @history 2011-05-03 Jeannie Walldren - Removed Mgs namespace wrap.
   *
   */
  class MocWideAngleDistortionMap : public CameraDistortionMap {
    public:
      MocWideAngleDistortionMap(Camera *parent, bool red);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      std::vector<double> p_coefs;
      std::vector<double> p_icoefs;
      double p_scale;
      int p_numCoefs;
  };
};
#endif
