#include "MocWideAngleDistortionMap.h"

using namespace std;
namespace Isis {
  namespace Mgs {
    /** Constructor for MocWideAngleDistortionMap class
     * 
     * Define the distortion model coefficients for a Moc Wide
     * Angle camera.
     * 
     * @param parent    A pointer to the parent camera object
     * @param red       A flag indicating whether the filter is red or not
     *                  
     * @internal
     * 
     * @history 2005-02-01 Jeff Anderson Original version
     * @history 2007-02-24 Debbie A. Cook Changed to p_scale value for -1 to 1
     *                        to reflect the change to the affine coefficients
     *                        in the new MocAddendum003.ti file
     * 
     */
    MocWideAngleDistortionMap::MocWideAngleDistortionMap (Camera *parent, bool red) :
      CameraDistortionMap (parent,1.0) {
      // Set up distortion coefficients
      if (red) {
        p_coefs.push_back(0.9993258);
        p_coefs.push_back(0.4655529);
        p_coefs.push_back(-0.1548756);
        p_coefs.push_back(1.827967);
        p_coefs.push_back(-3.057435);
        p_coefs.push_back(2.226331);
        p_scale = 1.0;
        p_icoefs.push_back(0.9995458);
        p_icoefs.push_back(-0.4237090);
        p_icoefs.push_back(0.2810857);
        p_icoefs.push_back(-0.1697522);
        p_icoefs.push_back(0.068131536);
        p_icoefs.push_back(-0.012665644);
      }
      else {
        p_coefs.push_back(1.000246);
        p_coefs.push_back(0.4612695);
        p_coefs.push_back(0.2352545);
        p_coefs.push_back(0.3535922);
        p_coefs.push_back(-0.2853861);
        p_coefs.push_back(0.5574971);
        p_scale = 1.000452;
        p_icoefs.push_back(0.9994557);
        p_icoefs.push_back(-0.4515307);
        p_icoefs.push_back(0.3152195);
        p_icoefs.push_back(-0.1993053);
        p_icoefs.push_back(0.081707217);
        p_icoefs.push_back(-0.014814299);
      }
      p_numCoefs = 6;
    }


    bool MocWideAngleDistortionMap::SetFocalPlane(const double dx, 
                                                  const double dy) {
      // Apply scale factor
      p_focalPlaneX = dx;
      p_focalPlaneY = dy;
      double sdy = dy / p_scale;

      // See if we are close to the boresight which implies no distortion
      double s2 = dx * dx + sdy * sdy;
      if (s2 <= 1.0E-6) {
        p_undistortedFocalPlaneX = dx;
        p_undistortedFocalPlaneY = sdy;
        return true;
      }

      // Remove distortion
      double s = sqrt(s2);
      double ang = atan(s / p_camera->FocalLength());
      double ang2 = ang * ang;
      double angp = p_coefs[p_numCoefs-1];
      for (int i=p_numCoefs-2; i>=0; i--) {
        angp = angp * ang2 + p_coefs[i];
      }
      angp = angp * ang;
      double sp = p_camera->FocalLength() * tan(angp);
      p_undistortedFocalPlaneX = dx * sp / s;
      p_undistortedFocalPlaneY = sdy * sp / s;
      return true;
    }
    
    bool MocWideAngleDistortionMap::SetUndistortedFocalPlane(const double ux, 
                                                             const double uy) {
      p_undistortedFocalPlaneX = ux;
      p_undistortedFocalPlaneY = uy;

      // See if we are close to boresight
      double sp2 = ux * ux + uy * uy;
      if (sp2 <= 1.0E-6) {
        p_focalPlaneX = ux;
        p_focalPlaneY = uy * p_scale;
        return true;
      }

      // Add distortion
      double sp = sqrt(sp2);
      double angp = atan(sp / p_camera->FocalLength());
      double angp2 = angp * angp;
      double ang = p_icoefs[p_numCoefs-1];
      for (int i=p_numCoefs-2; i>=0; i--) { 
        ang = ang * angp2 + p_icoefs[i];
      }
      ang = ang * angp;
      double s = p_camera->FocalLength() * tan(ang);
      p_focalPlaneX = ux * s / sp;
      p_focalPlaneY = uy * s / sp * p_scale;
      return true;
    }
  }
}
