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
#include <cmath>

#include "iString.h"
#include "LroNarrowAngleDistortionMap.h"

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
  LroNarrowAngleDistortionMap::LroNarrowAngleDistortionMap(Camera *parent) : CameraDistortionMap(parent, 1) {
  }

  /**
   * @param naifIkCode 
   */
  void LroNarrowAngleDistortionMap::SetDistortion(const int naifIkCode) {
    std::string odkkey = "INS" + Isis::iString(naifIkCode) + "_OD_K";
    p_odk.clear();
    p_odk.push_back(p_camera->getDouble(odkkey, 0));
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
  bool LroNarrowAngleDistortionMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    double dk1 = p_odk[0];

    double den = 1 + dk1 * dy * dy;                 // r = dy*dy = distance from the focal plane center
    if(den == 0.0)
      return false;

    p_undistortedFocalPlaneX = dx;
    p_undistortedFocalPlaneY = dy / den;

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
   */
  bool LroNarrowAngleDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    double yt = uy;

    double rr, dr;
    double ydistorted;
    double yprevious = 1000000.0;
    double tolerance = 1.0e-10;

    bool bConverged = false;

    double dk1 = p_odk[0];

    // Owing to the odd distotion model employed in this senser if |y| is > 116.881145553046
    //  then there is no root to find.  Further, the greatest y that any measure on the sensor
    //  will acutally distort to is less than 20.  Thus, if any distorted measure is greater
    //  that that skip the iterations.  The points isn't in the cube, and exactly how far outside
    //  the cube is irrelevant.  Just let the camera model know its not in the cube....
    if (fabs(uy) > 40) {  //if the point is way off the image.....
      p_focalPlaneX = p_undistortedFocalPlaneX;
      p_focalPlaneY = 100.0;  //100.0 is >> 20.0, and clearly outside the cube
      return true;
    }

    // iterating to introduce distortion (in sample only)...
    // we stop when the difference between distorted coordinate
    // in successive iterations is at or below the given tolerance
    for(int i = 0; i < 50; i++) {
      rr = yt * yt;

      //  dr is the radial distortion contribution
      dr = 1.0 + dk1 * rr;

      // distortion at the current sample location
      yt = uy * dr;

      // distorted sample
      ydistorted = yt;

      if (yt < -1e121)  //debug
        break;  //debug

      // check for convergence
      if(fabs(yt - yprevious) <= tolerance) {
        bConverged = true;
        break;
      }

      yprevious = yt;
    }
    
    if(bConverged) {
      p_focalPlaneX = p_undistortedFocalPlaneX;
      p_focalPlaneY = ydistorted;
    }

    return bConverged;
  }
}
