#ifndef NewHorizonsLorriDistortionMap_h
#define NewHorizonsLorriDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis {
  /**
   * New Horizons LORRI Distortion Map
   *
   * @author 2014-06-14 Stuart Sides
   *
   * @internal
   *    @history 2014-06-08 Staurt Sides - Original version. Equations and coefficients
   *    taken from Jet Propulsion Laboratory Interoffice Memorandum 2011/06/08 "New Horizons
   *    LORRI Geometric Calibration of August 2006" From: W. M. Owen Jr. and D. O'Coonnell
   *
   *    @history 2016-02-24 Staurt Sides - New Horizons LORRI distortion model changed to
   *    subtract the distortion when going from distorted to undistorted instead of adding, and
   *    adding the distortion when going from undistorted to destorted.
   */
  class NewHorizonsLorriDistortionMap : public CameraDistortionMap {
    public:
      NewHorizonsLorriDistortionMap(Camera *parent, double e2, double e5, double e6,
                                    double zDirection = 1.0);
      ~NewHorizonsLorriDistortionMap() {};

      bool SetFocalPlane(const double ux, const double uy);
      bool SetUndistortedFocalPlane(const double dx, const double dy);

    private:
      double p_e2;
      double p_e5;
      double p_e6;
  };
};
#endif
