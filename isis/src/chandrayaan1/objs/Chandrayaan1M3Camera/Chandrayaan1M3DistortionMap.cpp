/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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

#include <iomanip>

#include "Chandrayaan1M3DistortionMap.h"
#include "CameraFocalPlaneMap.h"

using namespace std;

namespace Isis {
  Chandrayaan1M3DistortionMap::Chandrayaan1M3DistortionMap(Camera *parent,
                                                           double xp, double yp,
                                                           double k1, double k2, double k3,
                                                           double p1, double p2) :
                                                               CameraDistortionMap(parent, 1.0) {

    p_xp = xp;
    p_yp = yp;
    p_k1 = k1;
    p_k2 = k2;
    p_k3 = k3;
    p_p1 = p1;
    p_p2 = p2;
  }

  /**
   * Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x/y given a distorted focal plane x/y.
   * after calling this method, you can obtain the undistorted
   * x/y via the UndistortedFocalPlaneX and UndistortedFocalPlaneY methods
   *
   * @param dx distorted focal plane x in millimeters
   * @param dy distorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   */
  bool Chandrayaan1M3DistortionMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // reducing to principal point offset (xp,yp)
    double x = dx - p_xp;
    double y = dy - p_yp;

//    std::cout << setprecision(14) << " dx " << dx << " dy " << dy << " p_xp " << p_xp << " p_yp " << p_yp;
    // r is the distance between the principal point and the measured point on the image
    double rr = x * x + y * y;
//    std::cout << " rr " << rr << " r " << sqrt(rr);

    //  dr is the radial distortion contribution
    double dr = p_k1 + p_k2 * rr + p_k3 * rr * rr;
//    std::cout << " dr " << dr;

    // dtx and dty are the decentering distortion contribution in x and y
    double dtx = p_p1 * (rr + 2.0 * x * x) + 2.0 * p_p2 * x * y;
    double dty = 2.0 * p_p1 * x * y + p_p2 * (rr + 2 * y * y);
//    std::cout << " dtx " << dtx << " dty " << dty << std::endl;

    // image coordinates corrected for principal point, radial and decentering distortion (p1,p2,p3)
    p_undistortedFocalPlaneX = dx + x * dr + dtx;
    p_undistortedFocalPlaneY = dy + y * dr + dty;

    return true;
  }


  /**
   * Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   * After calling this method, you can obtain the distorted x/y via the
   * FocalPlaneX and FocalPlaneY methods
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   */
  bool Chandrayaan1M3DistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {

    double xt = ux;
    double yt = uy;

    double xx, yy, rr, dr;
    double xdistortion, ydistortion;
    double xdistorted, ydistorted;
    double xprevious, yprevious;
    
    //  dr is the radial distortion contribution
    //  dtx and dty are the decentering distortion contributions in x and y
   
    xprevious = 1000000.0;
    yprevious = 1000000.0;

    double tolerance = 0.000001;

    bool bConverged = false;

    // iterating to introduce distortion...
    // we stop when the difference between distorted coordinates
    // in successive iterations is at or below the given tolerance
    for (int i = 0; i < 50; i++) {
      xx = xt * xt;
      yy = yt * yt;
      rr = xx + yy;

      // radial distortion
      //  dr is the radial distortion contribution
      // -dt*sin(p_t0) is the decentering distortion contribution in the x-direction
      //  dt*cos(p_t0) is the decentering distortion contribution in the y-direction
      dr = p_k1 + p_k2 * rr + p_k3 * rr * rr;

      double dtx = p_p1 * (rr + 2.0 * xt * xt) + 2.0 * p_p2 * xt * yt;
      double dty = 2.0 * p_p1 * xt * yt + p_p2 * (rr + 2 * yt * yt);

      // distortion at the current point location
      xdistortion = dr * xt + dtx;
      ydistortion = dr * yt + dty;

      // updated image coordinates
      xt = ux - xdistortion;
      yt = uy - ydistortion;

      // distorted point corrected for principal point
      xdistorted = xt + p_xp;
      ydistorted = yt + p_yp;

      // check for convergence
      if ((fabs(xt - xprevious) <= tolerance) && (fabs(yt - yprevious) <= tolerance)) {
        bConverged = true;
        break;
      }

      xprevious = xt;
      yprevious = yt;
    }

    if (bConverged) {
      p_undistortedFocalPlaneX = ux;
      p_undistortedFocalPlaneY = uy;

      p_focalPlaneX = xdistorted;
      p_focalPlaneY = ydistorted;
    }

    return bConverged;
  }
}
