/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NewHorizonsLorriDistortionMap.h"
#include "CameraFocalPlaneMap.h"

using namespace std;

namespace Isis {

  /**
   *
   * Constructs a Distortion Map object for the New Horizons LORRI Camera.
   *
   * @param parent Pointer to parent Camera object
   * @param e2
   * @param e2
   * @param e2
   * @param zDirection passed on to the parrent
   *
   * @internal
   *   @history 2014-06-15 Stuart Sides - Original version
   */

  NewHorizonsLorriDistortionMap::NewHorizonsLorriDistortionMap(Camera *parent, double e2, double e5,
      double e6, double zDirection) : CameraDistortionMap(parent, zDirection) {
    p_e2 = e2;
    p_e5 = e5;
    p_e6 = e6;
  }


  /**
   * Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x/y given a distorted focal plane x/y. After calling this
   * method, you can obtain the undistorted x/y via the UndistortedFocalPlaneX and
   * UndistortedFocalPlaneY methods
   *
   * @param dx Distorted focal plane x, in millimeters
   * @param dy Distorted focal plane y, in millimeters
   *
   * @return whether the conversion was successful
   */
  bool NewHorizonsLorriDistortionMap::SetFocalPlane(const double dx, const double dy) {

    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // Reducing to the principle point offset (xp,yp)
    double x = dx;
    double y = dy;

    // r is the distance between the principal point and the measured point on the image
    double rr = x * x + y * y;

    // dr is the radial distortion contribution
    // The equation below was changed from all + to all - to adjust the distortion model to fit
    // the definition of the LORRI distortion. The original version with +, was defined from Bill
    // Owen's paper with an assumption the xs and ys in the equations were distorted x,ys. After
    // meeting with the LORRI team +s were changed to -s to account for the x,ys actually being
    // undistorted focal plane positions. That is, the undistorted focal plane positions are
    // closer to the center of the image than the distorted focal plane positions.
    // NOTE: The discussions showed the Ky and e5 values needed to be negated. The e5 value has
    // now been negated in the LORRI IK, and the y is now negated in the equation below.
    // NOTE: The Y and Line values can not be negated in the transY and transL affines because
    // this would cause the p_xxxxx class member variables to be in a flipped (top to bottom)
    // coordinate system relative to the SPICE defined focal plane coordinate system.
    double dr = 1.0 - rr * p_e2 - y * p_e5 - x * p_e6;

    // Image coordinates corrected for distortion
    p_undistortedFocalPlaneX = x * dr;
    p_undistortedFocalPlaneY = y * dr;

    return true;

  }


  /**
   * Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   * After calling this method, you can obtain the distorted x/y via the
   * FocalPlaneX and FocalPlaneY methods
   *
   * @param ux Undistorted focal plane x, in millimeters
   * @param uy Undistorted focal plane y, in millimeters
   *
   * @return whether the conversion was successful
   */
  bool NewHorizonsLorriDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {

    // Image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    double xt = ux;
    double yt = uy;

    double xx, yy, xy, rr;
    double xdistortion, ydistortion;
    double xdistorted, ydistorted;
    double xprevious, yprevious;

    xprevious = 1000000.0;
    yprevious = 1000000.0;

    double tolerance = 0.000001;

    bool bConverged = false;

    // Iterating to introduce distortion...
    // We stop when the difference between distorted coordinates
    // in successive iterations is at or below the given tolerance
    for (int i = 0; i < 50; i++) {
      xx = xt * xt;
      yy = yt * yt;
      xy = xt * yt;
      rr = xx + yy;

      // Distortion at the current point location
      xdistortion = xt * rr * p_e2 + xy * p_e5 + xx * p_e6;
      ydistortion = yt * rr * p_e2 + yy * p_e5 + xy * p_e6;

      // Updated image coordinates
      // Changed to + instead of -. See comment in SetFocalPlane above
      xt = ux + xdistortion;
      yt = uy + ydistortion;

      // Distorted point corrected for principal point
      xdistorted = xt; // No PP for LORRI
      ydistorted = yt; // No PP for LORRI

      // Check for convergence
      if ((fabs(xt - xprevious) <= tolerance) && (fabs(yt - yprevious) <= tolerance)) {
        bConverged = true;
        break;
      }

      xprevious = xt;
      yprevious = yt;
    }

    if (bConverged) {
      p_focalPlaneX = xdistorted;
      p_focalPlaneY = ydistorted;
    }

    return bConverged;

  }
}
