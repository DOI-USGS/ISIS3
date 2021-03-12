#ifndef RadarSlantRangeMap_h
#define RadarSlantRangeMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "Camera.h"
#include "CameraDistortionMap.h"

namespace Isis {
  /** Convert between radar ground range and slant range
   *
   * Creates a map for converting radar ground range distance and
   * slant range distance
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2008-06-16 Jeff Anderson
   *
   * @internal
   *  @history 2009-07-01 Janet Barrett - Changed the bracketing method
   *                          used to solve for the root of the function that
   *                          determines ground range given slant range;
   *                          fixed code that determines the range coefficients
   *                          to used based on current ephemeris time
   *  @history 2010-03-19 Debbie A. Cook - Added comments about the units
   *                          and corrected slant in SetUndistortedFocalPlane
   *                          to be in meters instead of km
   *  @history 2011-09-14 Randy Kirk - Fixed the ComputeA method so that it
   *                          is handling the range coefficients properly. A
   *                          linear fit is used to obtain the range coefficients
   *                          if the current time falls between 2 points with
   *                          known range coefficients.
   *  @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   *  @history 2016-02-24 Randy Kirk and Janet Barrett - Fixed an issue that caused
   *                          the sensor model for LRO and Chandrayaan's MiniRF to not be able to 
   *                          calculate and report lat/lon in the LXB mode. References #2400.
   *  @history 2016-08-01 Kristin Berry - Added the ability to extend the range of the initial root
   *                          bracket in SetUndistortedFocalPlan if the initial range was too 
   *                          narrow. Also added RAs & DECs to the camera model.References #2400.
   *  @history 2018-09-28 Kaitlyn Lee - Removed unnecessary lines of code that were
   *                          causing build warnings on MacOS 10.13. Updated code up to standards. 
   *                          References #5520. 
   *                      
   */
  class RadarSlantRangeMap : public CameraDistortionMap {
    public:
      RadarSlantRangeMap(Camera *parent, double groundRangeResolution);

      //! Destructor
      virtual ~RadarSlantRangeMap() {};

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

      void SetCoefficients(PvlKeyword &keyword);

      void SetWeightFactors(double range_sigma, double doppler_sigma);

    private:
      void ComputeA();
      double p_et;
      double p_a[4];
      std::vector<double> p_time;
      std::vector<double> p_a0;
      std::vector<double> p_a1;
      std::vector<double> p_a2;
      std::vector<double> p_a3;

      int p_maxIterations;
      double p_tolerance;
      double p_initialMinGroundRangeGuess;
      double p_initialMaxGroundRangeGuess;

      double p_rangeSigma;
      double p_dopplerSigma;
      Camera *p_camera;
  };
};
#endif
