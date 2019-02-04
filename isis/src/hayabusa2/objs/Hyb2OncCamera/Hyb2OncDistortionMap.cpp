/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2008/02/21 16:04:33 $
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

    double x = dx;
    double y = dy;

    double r2 = (x * x) + (y * y);
    double r4 = r2*r2;
    
    // Apply distortion correction
    // https://www.darts.isas.jaxa.jp/pub/hayabusa2/onc_bundle/browse/
    // r2 = x^2 + y^2
    // rprime = L1*r + L2*r^3 + L3*r^5, where Li are distortion coeffs
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
   */
  bool Hyb2OncDistortionMap::SetUndistortedFocalPlane(const double ux,
                                                      const double uy) {

    // Image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

// TEMP GET IT COMPILING REMOVE BEFORE REAL WORK
    if (1 == 1) {
      p_focalPlaneX = ux;
      p_focalPlaneY = uy;
      return true;
    }

    double xt = ux;
    double yt = uy;

    double xx, yy, rr, rrrr, dr;
    double xdistortion, ydistortion;
    double xdistorted, ydistorted;
    double xprevious, yprevious;

    xprevious = 1000000.0;
    yprevious = 1000000.0;

    double tolerance = 0.000001;

    bool bConverged = false;

    // Iterating to introduce distortion...
    // We stop when the difference between distorted coordinates
    // in successive iterations is below the given tolerance
    for (int i = 0; i < 50; i++) {
      xx = xt * xt;
      yy = yt * yt;
      rr = xx + yy;
      rrrr = rr * rr;

      // Radial distortion
      // dr is the radial distortion contribution
      dr = p_odk[0] + p_odk[1] * rr + p_odk[2] * rrrr;

      // Distortion at the current point location
      xdistortion = xt * dr;
      ydistortion = yt * dr;

      // updated image coordinates
      xt = ux - xdistortion;
      yt = uy - ydistortion;

      // distorted point corrected for principal point
      xdistorted = xt;
      ydistorted = yt;

      // check for convergence
      if ((fabs(xt - xprevious) < tolerance) && (fabs(yt - yprevious) < tolerance)) {
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

