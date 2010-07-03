using namespace std;

#include <cmath>
#include "ThemisIrDistortionMap.h"

namespace Isis {
  namespace Odyssey {
    ThemisIrDistortionMap::ThemisIrDistortionMap (Camera *parent) :
      CameraDistortionMap (parent,1.0) {
      SetBand(1);
      p_alpha1 = 0.00447623;  // Currently not used
      p_alpha2 = 0.00107556;  // Disabled Y portion of optical distortion
    }

    void ThemisIrDistortionMap::SetBand (const int band) {
      if (band < 1 || band > 10) {
        string msg = "Band number out of array bounds in ThemisIRDistortionMap";
        throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
      }
      double k[] = { 0.996005, 0.995358, 0.994260, 0.993290, 0.992389,
                     0.991474, 0.990505, 0.989611, 0.988653, 0.9877 };

      p_k = k[band-1];
    }


    bool ThemisIrDistortionMap::SetFocalPlane(const double dx,
                                              const double dy) {
      p_focalPlaneX = dx;
      p_focalPlaneY = dy;

      p_undistortedFocalPlaneX = p_focalPlaneX / p_k;

      double radical = (1.0 + p_alpha1) * (1.0 + p_alpha1) + 4.0 * p_alpha2 * dy;
      if (radical < 0.0) return false;
      radical = sqrt(radical);
      double denom = 1.0 + p_alpha1 + radical;
      if (denom == 0.0) return false;
      p_undistortedFocalPlaneY = 2.0 * dy / denom;

      return true;
    }

    bool ThemisIrDistortionMap::SetUndistortedFocalPlane(const double ux,
                                                         const double uy) {
      p_undistortedFocalPlaneX = ux;
      p_undistortedFocalPlaneY = uy;

      p_focalPlaneX = p_undistortedFocalPlaneX * p_k;

      p_focalPlaneY = p_undistortedFocalPlaneY +
                      p_alpha1*p_undistortedFocalPlaneY +
                      p_alpha2*p_undistortedFocalPlaneY * p_undistortedFocalPlaneY;

      return true;
    }
  }
}
