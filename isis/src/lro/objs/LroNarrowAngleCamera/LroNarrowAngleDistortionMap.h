#ifndef LroNarrowAngleDistortionMap_h
#define LroNarrowAngleDistortionMap_h

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
   * from the focal plane of a camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @see LroNarrowAngleCamera
   *
   * @author 2009-07-03 Jacob Danton
   * @internal
   *   @history 2010-05-10 Ken Edmundson - Corrected computation of distorted
   *            and undistorted locations
   *   @history 2010-08-21 Kris Becker - Changed the sign of the distortion
   *            parameter to match the calibration report.  The LRO/LROC IK
   *            lro_lroc_v14.ti and above contain the appropriate parameters
   *            to coincide with the code change made here.  IMPORTANT:  This
   *            results in Version = 2 of the LroNarrowAngleCamera as depicted
   *            in the Camera.plugin for both NAC-L and NAC-R.
   *   @history 2011-05-03 Jeannie Walldren - Removed Lro namespace wrap.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   */
  class LroNarrowAngleDistortionMap : public CameraDistortionMap {
    public:
      LroNarrowAngleDistortionMap(Camera *parent);

      //! Destroys the LroNarrowAngleDistortionMap object.
      virtual ~LroNarrowAngleDistortionMap() {};

      void SetDistortion(const int naifIkCode);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

  };
};
#endif
