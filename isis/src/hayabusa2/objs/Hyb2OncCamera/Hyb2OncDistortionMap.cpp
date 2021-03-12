/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>
#include <QtGlobal>
#include <QtMath>

#include "Hyb2OncDistortionMap.h"

namespace Isis {
  /**
   * Hayabusa 2 ONC Camera distortion map constructor
   *
   * Create a camera distortion map for Hayabusa 2's ONC-T, ONC-W1, and
   * ONC-W2. This class maps between distorted and undistorted
   * focal plane x/y's. The default mapping is the identity, that is,
   * the focal plane x/y and undistorted focal plane x/y will be
   * identical.
   *
   * @param parent        the parent camera that will use this distortion map
   * @param zDirection    the direction of the focal plane Z-axis
   *                      (either 1 or -1)
   *
   */
  Hyb2OncDistortionMap::Hyb2OncDistortionMap(Camera *parent, double zDirection)
      : CameraDistortionMap(parent, zDirection) {
  }


  /**
   * Destructor
   */
  Hyb2OncDistortionMap::~Hyb2OncDistortionMap() {
  }


  /**
   * Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x/y given a distorted focal plane x/y.
   * This virtual method can be used to apply various techniques for removing
   * optical distortion in the focal plane of a camera.  The default
   * implementation uses a polynomial distortion if the SetDistortion method
   * is invoked.  After calling this method, you can obtain the undistorted
   * x/y via the UndistortedFocalPlaneX and UndistortedFocalPlaneY methods
   *
   * @param dx distorted focal plane x in millimeters
   * @param dy distorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   * @todo Generalize polynomial equation
   */
  bool Hyb2OncDistortionMap::SetFocalPlane(double dx, double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // reducing to principal point offset (xp,yp)
    double x = dx;// - p_xp;
    double y = dy;// - p_yp;
    //
    // Get the distance from the focal plane center and if we are close
    // then skip the distortion (this prevents division by zero)
    double r = (x * x) + (y * y);
    double r2 = r*r;
    double r4 = r2*r2;
    if (r <= 1.0E-6) {
      p_undistortedFocalPlaneX = dx;
      p_undistortedFocalPlaneY = dy;
      return true;
    }

    // apply distortion correction
    // r = x^2 + y^2
    // rprime = L0*r + L1*r^3 + L2*r^5, where Li are distortion coeffs
    // "dr" is rprime divided by r, used to reduce operations
    double dr = p_odk[0] + p_odk[1] * r2 + p_odk[2] * r4;
    p_undistortedFocalPlaneX = dr * x;
    p_undistortedFocalPlaneY = dr * y;
    return true;
  }


  /**
   * Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   * This virtual method is used to apply various techniques for adding
   * optical distortion in the focal plane of a camera.  The default
   * implementation of this virtual method uses a polynomial distortion if
   * the SetDistortion method was invoked.
   * After calling this method, you can obtain the distorted x/y via the
   * FocalPlaneX and FocalPlaneY methods
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   * @todo Generalize polynomial equation
   * @todo Figure out a better solution for divergence condition
   */
  bool Hyb2OncDistortionMap::SetUndistortedFocalPlane(const double ux,
      const double uy) {
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    // Compute the distance from the focal plane center and if we are
    // close to the center then no distortion is required

    bool converged = false;
    int iteration = 0;
    double tolMilliMeters = p_camera->PixelPitch() / 100.0;
    double x = ux;
    double y = uy;
    double r = (x * x) + (y * y);
    if (r <= 1.0E-6) {
      p_focalPlaneX = ux;
      p_focalPlaneY = uy;
      return true;
    }
    double rPrevious = r;

    while (!converged && qAbs(r - rPrevious) > tolMilliMeters) {
      double r2 = r*r;
      double r4 = r2*r2;
      double dr = p_odk[0] + p_odk[1] * r2 + p_odk[2] * r4;
      rPrevious = r;
      x = dr * x;
      y = dr * y;
      r = x*x + y*y;

      iteration++;
      if (iteration > 50) {
        converged = false;
        break;
      }
    }

    converged = true;
    p_focalPlaneX = x;
    p_focalPlaneY = y;

    return converged;
  }

}
