#ifndef TaylorCameraDistortionMap_h
#define TaylorCameraDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef CameraDistortionMap_h
#include "CameraDistortionMap.h"
#endif
#include "Camera.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.  This class describes a non-radial
   * distortion map. The distortion map is a third-order Taylor series expansion
   * of a generic function.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Messenger
   *
   * @see MdisCamera
   * @see CameraDistortionMap
   *
   * @author ????-??-?? Kris Becker
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added Isis disclaimer.
   *   @history 2011-05-23 Janet Barrett and Steven Lambright - Spice::GetDouble
   *                         is no longer a static call.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                         coding standards. References #972
   */
  class TaylorCameraDistortionMap : public CameraDistortionMap {
    public:
      TaylorCameraDistortionMap(Camera *parent, double zDirection = 1.0);

      void SetDistortion(const int naifIkCode);

      //! Destructor
      ~TaylorCameraDistortionMap() {};

      bool SetFocalPlane(const double dx, const double dy);

      bool SetUndistortedFocalPlane(const double ux, const double uy);

    protected:
      std::vector<double> p_odtx; //!< distortion x coefficients
      std::vector<double> p_odty; //!< distortion y coefficients

      void DistortionFunction(double ux, double uy, double *dx, double *dy);
      void DistortionFunctionJacobian(double x, double y, double *Jxx, double *Jxy, double *Jyx, double *Jyy);
  };
};
/**
 * Please direct questions to
 * Lillian Nguyen, JHUAPL, (443)778-5477, Lillian.Nguyen@jhuapl.edu
 */
#endif /*TaylorCameraDistortionMap_h*/
