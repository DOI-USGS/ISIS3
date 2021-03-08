#ifndef LoMediumDistortionMap_h
#define LoMediumDistortionMap_h

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
   * from the focal plane of the Lunar Orbiter medium resolution camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarOrbiter
   *
   * @see LoMediumCamera
   *
   * @author 2007-07-31 Debbie A. Cook
   *
   * @internal
   *
   *   @history 2007-07-31 Debbie A. Cook - Original version
   *   @history 2008-02-04 Jeff Anderson - Made change to support variable
   *                          focal length in THEMIS IR camera
   *   @history 2008-07-25 Steven Lambright - Fixed constructor;
   *                          CameraDistortionMap is responsible both for
   *                          setting the p_camera protected member and calling
   *                          Camera::SetDistortionMap. When the parent called
   *                          Camera::SetDistortionMap the Camera took ownership
   *                          of the instance of this object. By calling this
   *                          twice, and with Camera only supporting having one
   *                          distortion map, this object was deleted before the
   *                          constructor was finished.
   *   @history 2009-05-22 Debbie A. Cook - Cleaned up code and added iteration
   *                          loop. Previous version only iterated twice, but
   *                          results indicated more iterations were needed for
   *                          better accuracy.
   *   @history 2009-08-21 Debbie A. Cook - Added test for data outside focal
   *                          plane limits plus 10% to avoid getting erroneous
   *                          data projected on oblique images
   *   @history 2010-01-25 Debbie A. Cook - Increased out-of-bounds test to
   *                          17.5% of fiducial max to make sure lat/lons were
   *                          defined to the image edges
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation. Removed Lo
   *                          namespace wrap.
   *   @history 2012-07-06 Debbie A. Cook - Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   *   @history 2013-02-22 Debbie A. Cook - Updated SetUndistortedFocalPlane method to reflect
   *                          correction made to LookCtoFocalPlaneXY in CameraGroundMap.   The
   *                          adjustment for the z direction occurs in CameraGroundMap and is no
   *                          needed here.  Fixes Mantis ticket #1524
   */
  class LoMediumDistortionMap : public CameraDistortionMap {
    public:
      LoMediumDistortionMap(Camera *parent);

      void SetDistortion(const int naifIkCode);
      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double p_sample0;              //!< Center of distortion on sample axis
      double p_line0;                //!< Center of distortion on line axis
      std::vector<double> p_coefs;   //!< Distortion coeficients
      std::vector<double> p_icoefs;  //!< Distortion coeficients
  };
};
#endif
