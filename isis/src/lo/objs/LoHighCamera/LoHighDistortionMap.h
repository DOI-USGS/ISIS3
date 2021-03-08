#ifndef LoHighDistortionMap_h
#define LoHighDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  /**
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the Lunar Orbiter high resolution camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarOrbiter
   *
   * @see LoHighCamera
   *
   * @author 2007-06-29 Debbie A. Cook
   *
   * @internal
   *
   *   @history 2007-06-29 Debbie A. Cook - Original version
   *   @history 2008-02-04 Jeff Anderson - Made change to allow for variable
   *                          focal length in THEMIS IR
   *   @history 2008-07-25 Steven Lambright - Fixed constructor;
   *                          CameraDistortionMap is responsible both for
   *                          setting the p_camera protected member and calling
   *                          Camera::SetDistortionMap. When the parent called
   *                          Camera::SetDistortionMap the Camera took ownership
   *                          of the instance of this object. By calling this
   *                          twice, and with Camera only supporting having one
   *                          distortion map, this object was deleted before the
   *                          constructor was finished.
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation. Removed Lo
   *                          namespace wrap.
   *   @history 2012-07-06 Debbie A. Cook - Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   *   @history 2013-02-22 Debbie A. Cook - Updated SetUndistortedFocalPlane method to reflect
   *                          correction made to LookCtoFocalPlaneXY in CameraGroundMap.   The
   *                          adjustment for the z direction occurs in CameraGroundMap and is no
   *                          needed here.  Fixes Mantis ticket #1524
   */
  class LoHighDistortionMap : public CameraDistortionMap {
    public:
      LoHighDistortionMap(Camera *parent);

      void SetDistortion(const int naifIkCode);
      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double p_xPerspective;         //!< Perspective correction factor in x
      double p_yPerspective;         //!< Perspective correction factor in y
      double p_x0;                   //!< Center of distortion on x axis
      double p_y0;                   //!< Center of distortion on y axis
      std::vector<double> p_coefs;   //!< Distortion coefficients
      std::vector<double> p_icoefs;  //!< Distortion coefficients
  };
};
#endif
