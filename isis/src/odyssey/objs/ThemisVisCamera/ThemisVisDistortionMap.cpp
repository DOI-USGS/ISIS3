
using namespace std;

#include "ThemisVisDistortionMap.h"

namespace Isis {
  namespace Odyssey {
   /**
    * Constructs a Distortion Map for the Themis Vis Camera
    * 
    * @param parent Pointer to the parent Camera object
    */
    ThemisVisDistortionMap::ThemisVisDistortionMap (Camera *parent) :
      CameraDistortionMap (parent,1.0) {
      // Set necessary constant values
      p_irPixelPitch = 0.05;
      p_visPixelPitch = 0.009;
      p_ir_b5 = (95.0 + 110.0) / 2.0;
    }

   /**
    * Sets the focal plane value for the distortion map
    * 
    * @param dx The focal plane x value
    * @param dy The focal plane y value
    * 
    * @return bool Returns true if the set was successful and false if it was 
    *              not
    */
    bool ThemisVisDistortionMap::SetFocalPlane(const double dx, 
                                              const double dy) {
      p_focalPlaneX = dx;
      p_focalPlaneY = dy;

      // Constant Values
      double od_icx[] = {-4.02919e-5,0.0,0.0};
      double od_icy[] = {-0.0176112,-0.00718251,5.52591e-5};

      // Calculate necessary intermediate values
      double jp = p_focalPlaneY / p_irPixelPitch;
      double deltajp = od_icy[0] + (od_icy[1] * (-jp)) + 
        (od_icy[2] * (-jp) * (-jp));
      double ip = p_focalPlaneX / p_irPixelPitch;
      double cb1 = od_icx[0] * ((-jp) + 109.5 - p_ir_b5 - deltajp);

      // Set undistorted focal plane x & y positions
      p_undistortedFocalPlaneX = ip * p_irPixelPitch * (1.0 + (cb1 / (1.0 - cb1)));
      p_undistortedFocalPlaneY = (jp - deltajp) * p_irPixelPitch;
      return true;
    }

   /**
    * Sets the undistorted focal plane value for the distortion map
    * 
    * @param ux The undistorted focal plane x value
    * @param uy The undistorted focal plane y value
    * 
    * @return bool Returns true if the set was successful and false if it was 
    *              not
    */
    bool ThemisVisDistortionMap::SetUndistortedFocalPlane(const double ux, 
                                                         const double uy) {
      p_undistortedFocalPlaneX = ux;
      p_undistortedFocalPlaneY = uy;

      // Constant values
      double od_cx[] = {-4.02919e-5,0.0,0.0};
      double od_cy[] = {-0.0178649,-0.00727843,5.65278e-5};

      // Calculate necessary intermediate values
      double j = p_undistortedFocalPlaneY / p_irPixelPitch;
      double deltaj = od_cy[0] + (od_cy[1] * (-j)) + (od_cy[2] * (-j) * (-j));
      double cb1 = od_cx[0] * ((-j) + 109.5 - p_ir_b5);

      // Set focal plane x & y values
      p_focalPlaneX = p_undistortedFocalPlaneX * (1.0 + cb1);
      p_focalPlaneY = p_irPixelPitch * (j + deltaj);
      return true;
    }
  }
}
