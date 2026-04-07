#include <cmath>
#include <exception>

#include <QString>

#include "Camera.h"
#include "CameraDistortionMap.h"
#include "IException.h"
#include "IString.h"

#include "ShadowCamDistortionMap.h"

namespace Isis {

  static constexpr double TOLERANCE = 1.0e-10;
  static constexpr double OUT_OF_BOUNDS_LIMIT = 40;

  ShadowCamDistortionMap::ShadowCamDistortionMap(Camera *parent) : CameraDistortionMap(parent, 1) {
    try {
      // Logging has been removed here
    }
    catch (const std::exception &e) {
      throw IException(IException::User, QString("Error initializing ShadowCamDistortionMap: %1").arg(e.what()), _FILEINFO_);
    }
  }


  void ShadowCamDistortionMap::SetDistortion(int naifIkCode) {
    try {
      QString odkkey = "INS" + toString(naifIkCode) + "_OD_K";
      p_odk.clear();
      p_odk.push_back(p_camera->getDouble(odkkey, 0));
    }
    catch (const std::exception &e) {
      throw IException(
        IException::User,
        QString("Error setting distortion coefficients for NAIF IK Code: %1 - %2").arg(naifIkCode).arg(e.what()), _FILEINFO_);
    }
  }

  bool ShadowCamDistortionMap::SetFocalPlane(double dx, double dy) {
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

  bool ShadowCamDistortionMap::SetUndistortedFocalPlane(double ux, double uy) {
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
      double yDistorted;
      double yPrevious = 1000000.0;
      bool converged = false;

      double dk1 = p_odk[0];

      for (int i = 0; i < 50; i++) {
        rr = yt * yt;
        dr = 1.0 + dk1 * rr;
        yt = uy / dr;
        yDistorted = yt;

        if (yt < -1e121) {
          break;  // Invalid distorted value, break the loop
        }

        if (fabs(yt - yPrevious) <= TOLERANCE) {
          converged = true;
          break;
        }

        yPrevious = yt;
      }

      if (converged) {
        p_focalPlaneX = p_undistortedFocalPlaneX;
        p_focalPlaneY = yDistorted;
      }

      return converged;
    }
    catch (const std::exception &e) {
      throw IException(IException::User, QString("Error in SetUndistortedFocalPlane method: %1").arg(e.what()), _FILEINFO_);
    }
  }
}
