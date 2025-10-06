#include "ShadowCamDistortionMap.h"

namespace Isis {

  const double TOLERANCE = 1.0e-10;
  const double OUT_OF_BOUNDS_LIMIT = 40;

  // Camera distortion map constructor
  ShadowCamDistortionMap::ShadowCamDistortionMap(Camera *parent) : CameraDistortionMap(parent, 1) {
    try {
      // Logging has been removed here
    }
    catch (const std::exception &e) {
      throw IException(IException::User, QString("Error initializing ShadowCamDistortionMap: %1").arg(e.what()), _FILEINFO_);
    }
  }

  // Set distortion coefficients using IK
  void ShadowCamDistortionMap::SetDistortion(const int naifIkCode) {
    try {
      QString odkkey = "INS" + toString(naifIkCode) + "_OD_K";
      p_odk.clear();
      p_odk.push_back(p_camera->getDouble(odkkey, 0));
    }
    catch (const std::exception &e) {
      throw IException(IException::User, QString("Error setting distortion coefficients for NAIF IK Code: %1 - %2")
                          .arg(naifIkCode).arg(e.what()), _FILEINFO_);
    }
  }

  // Compute undistorted focal plane x/y from distorted x/y
  bool ShadowCamDistortionMap::SetFocalPlane(const double dx, const double dy) {
    try {
      p_focalPlaneX = dx;
      p_focalPlaneY = dy;

      double dk1 = p_odk[0];
      double den = 1 + dk1 * dy * dy;

      if (den == 0.0) {
        return false; // Simply return if there is an issue
      }

      p_undistortedFocalPlaneX = dx;
      p_undistortedFocalPlaneY = dy * den;

      return true;
    }
    catch (const std::exception &e) {
      throw IException(IException::User, QString("Error in SetFocalPlane method: %1").arg(e.what()), _FILEINFO_);
    }
  }

  // Compute distorted focal plane x/y from undistorted x/y
  bool ShadowCamDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    try {
      p_undistortedFocalPlaneX = ux;
      p_undistortedFocalPlaneY = uy;

      if (fabs(uy) > OUT_OF_BOUNDS_LIMIT) {
        p_focalPlaneX = p_undistortedFocalPlaneX;
        p_focalPlaneY = 100.0;  // Skip correction, return true since it's out of bounds
        return true;
      }

      double yt = uy;
      double rr, dr;
      double ydistorted;
      double yprevious = 1000000.0;
      bool bConverged = false;

      double dk1 = p_odk[0];

      for (int i = 0; i < 50; i++) {
        rr = yt * yt;
        dr = 1.0 + dk1 * rr;
        yt = uy / dr;
        ydistorted = yt;

        if (yt < -1e121) {
          break;  // Invalid distorted value, break the loop
        }

        if (fabs(yt - yprevious) <= TOLERANCE) {
          bConverged = true;
          break;
        }

        yprevious = yt;
      }

      if (bConverged) {
        p_focalPlaneX = p_undistortedFocalPlaneX;
        p_focalPlaneY = ydistorted;
      }

      return bConverged;
    }
    catch (const std::exception &e) {
      throw IException(IException::User, QString("Error in SetUndistortedFocalPlane method: %1").arg(e.what()), _FILEINFO_);
    }
  }
}
