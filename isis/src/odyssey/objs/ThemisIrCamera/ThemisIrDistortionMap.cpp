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
#include "ThemisIrDistortionMap.h"

using namespace std;

namespace Isis {
  /**
   * @param parent
   */
  ThemisIrDistortionMap::ThemisIrDistortionMap(Camera *parent) :
    CameraDistortionMap(parent, 1.0) {
    SetBand(1);
    p_alpha1 = 0.00447623;  // Currently not used
    p_alpha2 = 0.00107556;  // Disabled Y portion of optical distortion
  }

  void ThemisIrDistortionMap::SetBand(const int band) {
    if(band < 1 || band > 10) {
      string msg = "Band number out of array bounds in ThemisIRDistortionMap";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    double k[] = { 0.996005, 0.995358, 0.994260, 0.993290, 0.992389,
                   0.991474, 0.990505, 0.989611, 0.988653, 0.9877
                 };

    p_k = k[band-1];
  }


  bool ThemisIrDistortionMap::SetFocalPlane(const double dx,
      const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    p_undistortedFocalPlaneX = p_focalPlaneX / p_k;

    double radical = (1.0 + p_alpha1) * (1.0 + p_alpha1) + 4.0 * p_alpha2 * dy;
    if(radical < 0.0) return false;
    radical = sqrt(radical);
    double denom = 1.0 + p_alpha1 + radical;
    if(denom == 0.0) return false;
    p_undistortedFocalPlaneY = 2.0 * dy / denom;

    return true;
  }

  bool ThemisIrDistortionMap::SetUndistortedFocalPlane(const double ux,
      const double uy) {
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    p_focalPlaneX = p_undistortedFocalPlaneX * p_k;

    p_focalPlaneY = p_undistortedFocalPlaneY +
                    p_alpha1 * p_undistortedFocalPlaneY +
                    p_alpha2 * p_undistortedFocalPlaneY * p_undistortedFocalPlaneY;

    return true;
  }
}

