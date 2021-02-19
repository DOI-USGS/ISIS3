#ifndef RadarGroundMap_h
#define RadarGroundMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"
#include "CameraGroundMap.h"
#include "SurfacePoint.h"

namespace Isis {

#ifndef RADAR_LOOK_DIR
  namespace Radar {
    enum LookDirection { Left, Right };
  }
#define RADAR_LOOK_DIR
#endif

  /** Convert between undistorted focal plane coordinate (slant range)
   *  and ground coordinates
   *
   * This class is used to convert between undistorted focal plane
   * coordinate (the slant range) and ground coordinates lat/lon. This
   * class handles the case of Radar instruments.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2008-06-16 Jeff Anderson
   *
   * @internal
   *  @history 2009-07-01 Janet Barrett - Fixed intrack, crosstrack, and radial
   *                      coordinate calculations; changed boundary check to use
   *                      radius instead of lat,lon; updated calculation of Doppler
   *                      shift
   *  @history 2009-10-30 Debbie A. Cook - Fixed rotation of velocity vector in
   *                      SetFocalPlane, SetGround, and ComputeXv
   *  @history 2009-11-10 Janet Barrett - Added iteration check to SetFocalPlane
   *                      to take care of situation where DEM radius does not
   *                      converge
   *  @history 2009-11-20 Janet Barrett - Added a check to the SetGround method
   *                      to determine what side of the spacecraft the focal
   *                      plane coordinate falls. This fixed the mirror image
   *                      problem that occurred when projecting the image.
   *  @history 2009-12-14 Debbie A. Cook - Added ComputeXY method
   *  @history 2010-03-19 Debbe A. Cook - added class members p_wavelength, p_lookB,
   *            p_sB, p_slantRange, and p_dopplerFreq; and methods SlantRangeSigma,
   *            WaveLength, DopplerSigma, SetSlantRangeSigma, GetXY,
   *            GetXYdPosition, and GetXYdPoint.  Removed method SetWeightFactors.
   *  @history 2010-08-05 Debbie A. Cook - changed to use Surface point class
   *  @history 2010-11-22 Debbie A. Cook - moved PointPartial call out of
   *                       GetdXYdPoint to allow BundleAdjust to avoid multiple
   *                       calls for every measure.  The application must call
   *                       PointPartial to get the body-fixed look vector
   *                       derivative prior to calling this method.
   *  @history 2010-12-17 Debbie A. Cook - Corrected units to kilometers in GetXY
   *  @history 2012-01-03 Janet Barrett - Got rid of call to SetLookDirection in the
   *                      SetFocalPlane method. The call to SetLookDirection was
   *                      duplicating the functionality of the SetFocalPlane method.
   *  @history 2012-01-04 Janet Barrett - Added check for valid radius in the 
   *                      SetGround method.
   *  @history 2012-01-27 Janet Barrett - The iterative algorithm in the SetFocalPlane
   *                      method was failing on some points when using the DEM shape
   *                      model. To fix this problem, I decreased the tolerance from 1E-8 to
   *                      1E-6 and increased the number of iterations from 30 to
   *                      100.
   *  @history 2012-04-03 Janet Barrett - The iteration code was moved to its own method so 
   *                      that it can be run multiple times if necessary. The first iteration 
   *                      should suffice for those pixels that have shallow slopes. For those 
   *                      pixels that lie on steep slopes (up to 2x the incidence angle), then
   *                      an additional iteration call is needed. In the future, we may need to 
   *                      add more calls to the iteration method if the slope is greater than 2x 
   *                      the incidence angle. The slope variable will need to be halved each time 
   *                      the iteration method is called until a solution is found. So, for example, 
   *                      if we needed to call the iteration method a third time, the slope variable 
   *                      would be set to .25.
   *  @history 2012-04-11 Janet Barrett - Removed call to SetLookDirection from the SetGround
   *                      method.
   *  @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                      coding standards. References #972.
   * @history 2013-02-11 Debbie A. Cook, Fixed SetGround method by adding call to set the surface point 
   *                       in the ShapeModel so that photometric angles can be calculated.  References #775
   * @history 2016-07-19 Kristin Berry, Updated SetGround to call p_camera->Sensor::SetGround so that 
   *                       RA, DEC values will be set on level 2 images. References #2400.  
   *  @history 2019-05-15 Debbie A. Cook - Added optional bool argument to match parent GetXY 
   *                          method to allow the bundle adjustment to skip the back of planet test during 
   *                          iterations. References #2591.
   */
  class RadarGroundMap : public CameraGroundMap {
    public:
      RadarGroundMap(Camera *parent, Radar::LookDirection ldir, double waveLength);

      //! Destructor
      virtual ~RadarGroundMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz);
      virtual bool SetGround(const Latitude &lat, const Longitude &lon);
      virtual bool SetGround(const SurfacePoint &surfacePoint);
      virtual bool GetXY(const SurfacePoint &spoint, double *cudx,
                         double *cudy, bool test=false);
      virtual bool GetdXYdPosition(const SpicePosition::PartialType varType,
                                   int coefIndex, double *cudx, double *cudy);
      virtual bool GetdXYdPoint(std::vector<double> d_lookB, double *dx,
                                double *dy);

      //!Set the range sigma
      void SetRangeSigma(double rangeSigma) {
        p_rangeSigma = rangeSigma;
      };

      //! Return the range sigma
      double RangeSigma() {
        return p_rangeSigma;
      };

      //! Set the doppler sigma
      void SetDopplerSigma(double dopplerSigma) {
        p_dopplerSigma = dopplerSigma;
      };

      //! Return the doppler sigma
      double YScale() {
        return p_dopplerSigma;
      };

      //! Return the wavelength
      double WaveLength() {
        return p_waveLength;
      };

    private:
      double ComputeXv(SpiceDouble X[3]);
      double GetRadius(const Latitude &lat, const Longitude &lon);

      bool Iterate(SpiceDouble &R, const double &slantRangeSqr, const SpiceDouble c[],
                   const SpiceDouble r[], SpiceDouble X[], SpiceDouble &lat,
                   SpiceDouble &lon, const std::vector<double> &Xsc, const bool &useSlopeEqn,
                   const double &slope);

      Radar::LookDirection p_lookDirection;
      double p_tolerance;
      double p_slantRange;         //!< units are km
      double p_dopplerFreq;        //!< units are hertz
      double p_timeTolerance;
      double p_rangeSigma;         //!< Scaling factor to convert meters to focal plane coord
      double p_dopplerSigma;       //!< Scaling factor to convert hertz to focal plane coord
      double p_waveLength;         // km/sec/hertz ??
      std::vector<double> p_lookB;
      std::vector<double> p_sB;
      //std::vector<double> p_Xsc(3); //!< body fixed position
      double p_groundSlantRange;   //!< units are km
      double p_groundDopplerFreq;  //!< units are hertz

      Camera *p_camera;
  };
};
#endif
