/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef KaguyaTcCameraDistortionMap_h
#define KaguyaTcCameraDistortionMap_h

#include "CameraDistortionMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates for Kaguya's TC cameras.
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2019-04-04 Kristin Berry
   *
   * @internal
   *   @history 2019-04-04 Kristin Berry - Original version.
   *
   */
  class KaguyaTcCameraDistortionMap : public CameraDistortionMap {
    public:
      KaguyaTcCameraDistortionMap(Camera *parent, int naifIkCode);

      virtual ~KaguyaTcCameraDistortionMap();

      virtual bool SetFocalPlane(double dx, double dy);

      virtual bool SetUndistortedFocalPlane(double ux, double uy);

    protected:
      std::vector<double> p_odkx; //!< distortion x coefficients
      std::vector<double> p_odky; //!< distortion y coefficients
  };
};
#endif
