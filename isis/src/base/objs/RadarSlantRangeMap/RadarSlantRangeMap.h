/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/03/27 06:56:17 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#ifndef RadarSlantRangeMap_h
#define RadarSlantRangeMap_h

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
