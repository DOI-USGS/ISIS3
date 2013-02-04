/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/05/12 23:28:12 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <cmath>

#include "IString.h"
#include "LroWideAngleCameraDistortionMap.h"

namespace Isis {
  /** Camera distortion map constructor
   *
   * Create a camera distortion map.  This class maps between distorted
   * and undistorted focal plane x/y's.  The default mapping is the
   * identity, that is, the focal plane x/y and undistorted focal plane
   * x/y will be identical.
   *
   * @param parent        the parent camera that will use this distortion map
   * @param zDirection    the direction of the focal plane Z-axis
   *                      (either 1 or -1)
   *
   */
  LroWideAngleCameraDistortionMap::LroWideAngleCameraDistortionMap(Camera *parent, int naifIkCode) : CameraDistortionMap(parent) {
    QString odkkey = "INS" + toString(naifIkCode) + "_OD_K";
    for(int i = 0; i < 2; i++) {
      p_odk.push_back(p_camera->getDouble(odkkey, i));
    }
  }

  /** Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x/y given a distorted focal plane x/y.
   *
   * @param dx distorted focal plane x in millimeters
   * @param dy distorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   * @todo Generalize polynomial equation
   */

  bool LroWideAngleCameraDistortionMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    double dk1 = p_odk[0];
    double dk2 = p_odk[1];

    double rr = dx * dx + dy * dy;
    double r = sqrt(rr);

    double dr = 1.0 + dk1 * rr + dk2 * r * rr;
    if(dr == 0.0)
      return false;

    // Compute the undistorted positions
    p_undistortedFocalPlaneX = dx / dr;
    p_undistortedFocalPlaneY = dy / dr;

    return true;
  }

  /** Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   * @todo Generalize polynomial equation
   * @todo Figure out a better solution for divergence condition
   */

  bool LroWideAngleCameraDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    double xt = ux;
    double yt = uy;

    double r, rr, dr;
    double xdistorted, ydistorted;
    double xprevious, yprevious;

    xprevious = 1000000.0;
    yprevious = 1000000.0;
    double tolerance = 1.0e-10;

    bool bConverged = false;

    double dk1 = p_odk[0];
    double dk2 = p_odk[1];

    // iterating to introduce distortion...
    // we stop when the difference between distorted coordinates
    // in successive iterations is at or below the given tolerance
    for(int i = 0; i < 50; i++) {
      rr = xt * xt + yt * yt;
      r = sqrt(rr);

      // dr is the radial distortion contribution
      dr = 1.0 + dk1 * rr + dk2 * r * rr;

      // introducing distortion
      xt = ux * dr;
      yt = uy * dr;

      // distorted point
      xdistorted = xt;
      ydistorted = yt;

      // check for convergence
      if((fabs(xt - xprevious) <= tolerance) && (fabs(yt - yprevious) <= tolerance)) {
        bConverged = true;
        break;
      }

      xprevious = xt;
      yprevious = yt;
    }

    if(bConverged) {
      p_focalPlaneX = xdistorted;
      p_focalPlaneY = ydistorted;
    }
    return bConverged;
  }
}
