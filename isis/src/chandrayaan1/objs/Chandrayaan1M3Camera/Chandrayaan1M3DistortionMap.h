#ifndef Chandrayaan1M3DistortionMap_h
#define Chandrayaan1M3DistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "Camera.h"
#include "CameraDistortionMap.h"

namespace Isis {

  /**
   * @brief Distortion map for the Chandrayaan1 M3 camera
   *
   * This class is used by the Chandrayaan1 M3 camera model as its distortion map. Equations
   * provided by Randy Kirk and code provided by Ken Edmundson.
   *
   * @ingroup Camera
   *
   * @author ????-??-?? Ken Edmundson
   *
   * @internal
   *   @history 2013-11-24 Stuart Sides - Modified from ApolloMetricDistortionMap
   *   @history 2016-08-28 Kelvin Rodriguez - Removed unused member variable p_p3 to squash warnings
   *                              in clang. Part of porting to OS X 10.11
   *
   */

  class Chandrayaan1M3DistortionMap : public CameraDistortionMap {
    public:
      Chandrayaan1M3DistortionMap(Camera *parent, double xp, double yp,
                                  double k1, double k2, double k3,
                                  double p1, double p2);
      ~Chandrayaan1M3DistortionMap() {};

      bool SetFocalPlane(const double dx, const double dy);
      bool SetUndistortedFocalPlane(const double ux, const double uy);

    private: // parameters below are from camera calibration report
      double p_xp, p_yp;       //!< principal point coordinates
      double p_k1, p_k2, p_k3; //!< coefficients of radial distortion
      double p_p1, p_p2; //!< coefficients of decentering distortion
  };
};
#endif
