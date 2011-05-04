/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "ApolloMetricDistortionMap.h"
#include "CameraFocalPlaneMap.h"

using namespace std;
namespace Isis {
  /**
   *  
   * Constructs a Distortion Map object for the Apollo Metric Camera. 
   *  
   * @param parent Pointer to parent Camera object
   * @param xp Pricipal point x-coordinate
   * @param yp Pricipal point y-coordinate
   * @param k1 First coefficient of radial distortion
   * @param k2 Second coefficient of radial distortion
   * @param k3 Third coefficient of radial distortion
   * @param j1 First coefficient of decentering distortion
   * @param j2 Second coefficient of decentering distortion
   * @param t0 Angle between positive x-axis of image and vector to imaged point
   * 
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added documentation.
   */
  ApolloMetricDistortionMap::ApolloMetricDistortionMap(Camera *parent,
                                                       double xp, double yp,
                                                       double k1, double k2,
                                                       double k3, double j1,
                                                       double j2, double t0) :
                                                       CameraDistortionMap(parent) {
    p_xp = xp;
    p_yp = yp;
    p_k1 = k1;
    p_k2 = k2;
    p_k3 = k3;
    p_j1 = j1;
    p_j2 = j2;
    p_t0 = t0;
  }

  /**
   * Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x/y given a distorted focal plane x/y.
   * fter calling this method, you can obtain the undistorted
   * x/y via the UndistortedFocalPlaneX and UndistortedFocalPlaneY methods
   *
   * @param dx Distorted focal plane x, in millimeters
   * @param dy Distorted focal plane y, in millimeters
   *
   * @return whether the conversion was successful
   */
  bool ApolloMetricDistortionMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // reducing to principal point offset (xp,yp)
    double x = dx - p_xp;
    double y = dy - p_yp;

    // r is the distance between the principal point and the measured point on the image
    double rr = x * x + y * y;
    double rrrr = rr * rr;

    //  dr is the radial distortion contribution
    // -dt*sin(p_t0) is the decentering distortion contribution in the x-direction
    //  dt*cos(p_t0) is the decentering distortion contribution in the y-direction
    double dr = 1 + p_k1 * rr + p_k2 * rrrr + p_k3 * rr * rrrr;
    double dt = p_j1 * rr + p_j2 * rrrr;

    // image coordinates corrected for principal point, radial and decentering distortion
    p_undistortedFocalPlaneX = dr * x - dt * sin(p_t0);
    p_undistortedFocalPlaneY = dr * y + dt * cos(p_t0);

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
  bool ApolloMetricDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    double xt = ux;
    double yt = uy;

    double xx, yy, rr, rrrr, dr;
    double xdistortion, ydistortion;
    double xdistorted, ydistorted;
    double xprevious, yprevious;
    //  dr is the radial distortion contribution
    // -dt*sin(p_t0) is the decentering distortion contribution in the x-direction
    //  dt*cos(p_t0) is the decentering distortion contribution in the y-direction
    double dt;

    xprevious = 1000000.0;
    yprevious = 1000000.0;

    double tolerance = 0.000001;

    bool bConverged = false;

    // iterating to introduce distortion...
    // we stop when the difference between distorted coordinates
    // in successive iterations is at or below the given tolerance
    for(int i = 0; i < 50; i++) {
      xx = xt * xt;
      yy = yt * yt;
      rr = xx + yy;
      rrrr = rr * rr;

      // radial distortion
      //  dr is the radial distortion contribution
      // -dt*sin(p_t0) is the decentering distortion contribution in the x-direction
      //  dt*cos(p_t0) is the decentering distortion contribution in the y-direction
      dr = p_k1 * rr + p_k2 * rrrr + p_k3 * rr * rrrr;

      dt = p_j1 * rr + p_j2 * rrrr;

      // distortion at the current point location
      xdistortion = xt * dr - dt * sin(p_t0);
      ydistortion = yt * dr + dt * cos(p_t0);

      // updated image coordinates
      xt = ux - xdistortion;
      yt = uy - ydistortion;

      // distorted point corrected for principal point
      xdistorted = xt + p_xp;
      ydistorted = yt + p_yp;

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
