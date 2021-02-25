#ifndef ClementineUvvisDistortionMap_h
#define ClementineUvvisDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"
#include "CameraDistortionMap.h"

namespace Isis {

  /**
   * @brief Distortion map for the Clementine UVVIS camera
   *
   * This class is was copied from the Chandrayaan1 M3 camera model as its distortion map.
   * Equations provided by Randy Kirk and code provided by Ken Edmundson.
   *
   * @ingroup Camera
   *
   * @author 2017-12-27 Jeff Anderson & Victor Silva
   *
   * @internal
   *   @history 2017-12-27 Jeff Anderson & Victor Silva - Copied code from Chandrayaan1 M3 camera.
   *   @history 2018-09-01 Jeannie Backer - Added documentation and merged into public repo.
   */

  class ClementineUvvisDistortionMap : public CameraDistortionMap {
    public:
      ClementineUvvisDistortionMap(Camera *parent, double xp, double yp,
                                  double k1, double k2, double k3,
                                  double p1, double p2);
      ~ClementineUvvisDistortionMap();

      bool SetFocalPlane(const double dx, const double dy);
      bool SetUndistortedFocalPlane(const double ux, const double uy);

    private: // parameters below are from camera calibration report
      double p_xp; //!< Principal point x coordinate.
      double p_yp; //!< Principal point y coordinate.
      double p_k1; //!< Constant term coefficient of radial distortion.
      double p_k2; //!< Linear term coefficient of radial distortion.
      double p_k3; //!< Quadratic term coefficient of radial distortion.
      double p_p1; //!< First coefficient of decentering distortion.
      double p_p2; //!< Second coefficient of decentering distortion.
  };
};
#endif
