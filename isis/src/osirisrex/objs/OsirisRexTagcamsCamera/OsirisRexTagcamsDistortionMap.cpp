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
#include "OsirisRexTagcamsDistortionMap.h"
#include "CameraFocalPlaneMap.h"

using namespace std;
namespace Isis {
  /**
   *  
   * Constructs a Distortion Map object for the OSIRIS-REx TagCams Camera. 
   *  
   * @param parent Pointer to parent Camera object
   * @param k1 First coefficient of radial distortion
   * @param k2 Second coefficient of radial distortion
   * @param k3 Third coefficient of radial distortion
   * @param p1 x tangential distortion component
   * @param p2 y tangential distortion compoment
   * @param cx X distortion center pixel location (pixels)
   * @param cy Y distortion center pixel location (pixels)
   * 
   * @internal
   */
  OsirisRexTagcamsDistortionMap::OsirisRexTagcamsDistortionMap(Camera *parent,
                                                               int naifIkCode,
                                                               const double zdir)
                                                 : CameraDistortionMap(parent, zdir) {

    QString ikCode = toString(naifIkCode);
    QString odkKey = "INS" + ikCode + "_OPENCV_OD_K";
    QString ppKey  = "INS" + ikCode + "_OPENCV_PP";
    QString aoKey  = "INS" + ikCode + "_OPENCV_AXIS_ORIGIN";


    p_k1 = parent->getDouble(odkKey, 0);
    p_k2 = parent->getDouble(odkKey, 1);
    p_k3 = parent->getDouble(odkKey, 2);
    p_p1 = parent->getDouble(ppKey, 0);
    p_p2 = parent->getDouble(ppKey, 1);
    p_cx = parent->getDouble(aoKey, 0);
    p_cy = parent->getDouble(aoKey, 1);
    return;
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
  bool OsirisRexTagcamsDistortionMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // Compute the fx component. Rather than use separate focal length for
    // each axis assume it is averaged.
    // For normalizing the focal plane coordinates
    double pixel_pitch  = p_camera->PixelPitch();
    double focal_length = p_camera->FocalLength();

    double fxy =  focal_length / pixel_pitch;

    // Convert focal plane point to pixel coordinates to unitless x'/y'
    double x = ((dx / pixel_pitch) - p_cx) / fxy;
    double y = ((dy / pixel_pitch) - p_cy) / fxy;

    // r is radial component
    double rr = x * x + y * y;  // x.dot(x)
    double r = std::sqrt(r);
    double theta = std::atan(r);

    double theta2 = theta*theta, theta4 = theta2*theta2;

    //  dr is the radial distortion contribution
    double dr = 1 + (p_k1 * theta2) + (p_k2 * theta4) + (p_k3 * theta2 * theta4);

    // Tangential contributions
    double dt_x = 2.0 * p_p1 * x * y + p_p2 * theta2 + 2.0 * x * x;
    double dt_y = p_p1 * theta2 + p_p1 * 2.0 * y * y + 2.0 * p_p2 * x * y;

    // image coordinates corrected for principal point, radial and decentering distortion
    p_undistortedFocalPlaneX = (dr * x) + dt_x;
    p_undistortedFocalPlaneY = (dr * y) + dt_y;

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
  bool OsirisRexTagcamsDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    // Normalize HERE !!!
    double u_ux = ux;
    double u_uy = uy;

    double xt = u_ux;
    double yt = u_uy;

    double xx, yy, rr, rrrr, dr;
    double xdistortion, ydistortion;
    double xdistorted, ydistorted;
    double xprevious, yprevious;
    double dt_x, dt_y;
    //  dr is the radial distortion contribution

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
      dr = p_k1 * rr + p_k2 * rrrr + p_k3 * rr * rrrr;

      // Tangential
      dt_x = 2.0 * p_p1 * xt * yt + p_p2 * rr + 2.0 * xt * xt;
      dt_y = p_p1 * rr + p_p1 * 2.0 * yt * yt + 2.0 * p_p2 * xt * yt;

      // distortion at the current point location
      xdistortion = (xt * dr) + dt_x;
      ydistortion = (yt * dr) + dt_y;

      // updated image coordinates
      xt = u_ux - xdistortion;
      yt = u_uy - ydistortion;

      // distorted point corrected for principal point
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
