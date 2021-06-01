 /** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>
#include <iostream>

#include <boost/math/special_functions/legendre.hpp>

#include "Camera.h"
#include "CameraFocalPlaneMap.h"
#include "Constants.h"
#include "FunctionTools.h"
#include "IString.h"
#include "LineScanCameraDetectorMap.h"
#include "NewHorizonsMvicTdiCameraDistortionMap.h"

#include <QDebug>


using namespace boost::math;
using namespace std;
using namespace Isis;

namespace Isis {
  /** Camera distortion map constructor
   *
   * Create a camera distortion map.  This class maps between distorted
   * and undistorted focal plane x/y's.  The default mapping is the
   * identity, that is, the focal plane x/y and undistorted focal plane
   * x/y will be identical.
   *
   * @param parent                 the parent camera that will use this distortion map
   * @param zDirection             the direction of the focal plane Z-axis
   *                               (either 1 or -1)
   *
   * @param xDistortionCoeffs      distortion coefficients in x
   * @param yDistortionCoeffs      distortion coefficients in y
   * @param residualColDistCoeffs
   * @param residualRowDistCoeffs
   *
   */
  NewHorizonsMvicTdiCameraDistortionMap::NewHorizonsMvicTdiCameraDistortionMap(
      Camera *parent,
      vector<double> xDistortionCoeffs,
      vector<double> yDistortionCoeffs,
      vector<double> residualColDistCoeffs,
      vector<double> residualRowDistCoeffs) :
    CameraDistortionMap(parent, 1.0) {

    m_xDistortionCoeffs = xDistortionCoeffs;
    m_yDistortionCoeffs = yDistortionCoeffs;

    m_residualColDistCoeffs = residualColDistCoeffs;
    m_residualRowDistCoeffs = residualRowDistCoeffs;

    // half of detector in x is 32.5 mm
    m_focalPlaneHalf_x = 0.5 * p_camera->Samples() * p_camera->PixelPitch();
  }


  /** Destructor
   */
  NewHorizonsMvicTdiCameraDistortionMap::~NewHorizonsMvicTdiCameraDistortionMap() {
  }


  /** Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x given a distorted focal plane x
   *
   * TODO: Modify this - Distortion in line direction currently not considered (MVIC TDI is treated as line scan sensor)
   *
   * In the event of any failure, undistorted focal plane values are set equal to the raw
   * (distorted) values
   *
   * @param dx distorted focal plane x in millimeters
   * @param dy distorted focal plane y in millimeters
   *
   * @return success/failure
   *
   * @see SetDistortion
   */
  // TODO: Don't really like this - we always return true, even in event of failures
  bool NewHorizonsMvicTdiCameraDistortionMap::SetFocalPlane(const double dx, const double dy) {

    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // initialize undistorted focal plane coordinates to the distorted
    // coordinate values
    p_undistortedFocalPlaneX = dx;
    p_undistortedFocalPlaneY = dy;

    // if x lies outside of the detector, do NOT apply distortion
    // set undistorted focal plane values to be identical to raw values
    if ((fabs(dx) > m_focalPlaneHalf_x)) {
      return true;
    }

    // scale x to lie in the range -1.0 to +1.0
    // this is required for Legendre Polynomials, man
    double xscaled = -dx / m_focalPlaneHalf_x;

    // compute distortion corrections in x using Legendre Polynomials
    // these corrections are also in the -1.0 to +1.0 range
    double deltax1 = 0.0;
    if (!computeDistortionCorrections(xscaled, 0.0, deltax1)) {
      return true;
    }

    // now compute residual distortion corrections (per Jason Cook)
    // TODO: implementation not complete
    double deltax2 = 0.0;
    double deltay2 = 0.0;
    computeResidualDistortionCorrections(dx, deltax2, deltay2);

    // apply the corrections
    xscaled += deltax1;

    // scale back from range of '-1.0 to +1.0' to the detector, '-32.5 to 32.5 mm'
//    p_undistortedFocalPlaneX = -xscaled * m_focalPlaneHalf_x;
//    p_undistortedFocalPlaneY = p_focalPlaneY;
    p_undistortedFocalPlaneX = -xscaled * m_focalPlaneHalf_x + deltax2;
    p_undistortedFocalPlaneY = p_focalPlaneY + deltay2;

    return true;
  }


  /** Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   *
   * This is an iterative procedure as computing the inverse of the distortion equations used by New
   * Horizons MVIC is difficult.
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   */
  bool NewHorizonsMvicTdiCameraDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {

    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    double xt = ux;
    double yt = uy;

//    p_focalPlaneX = p_undistortedFocalPlaneX;
//    p_focalPlaneY = p_undistortedFocalPlaneY;
//    return true;

    // scale undistorted coordinates to range of -1.0 to +1.0
    double uxScaled = -ux/m_focalPlaneHalf_x;
    double xtScaled;
    double xprevious, yprevious;
    double scaledDeltax;
    double deltax2 = 0.0;
    double deltay2 = 0.0;

    xprevious = 1000000.0;
    yprevious = 1000000.0;

    double tolerance = 0.000001;

    bool bConverged = false;

    // iterating to introduce distortion...
    // we stop when the difference between distorted coordinates
    // in successive iterations is at or below the given tolerance
    for ( int i = 0; i < 50; i++ ) {

      xtScaled = -xt / m_focalPlaneHalf_x;

      // Changed this failure mode from a throw to return false.  These functions should never
      // throw, the callers are not catching, so bad things happen.  The callers should be checking
      // the return status
      if (fabs(xtScaled) > 1.0) {
        return bConverged;
      }

      // compute scaled distortion in x (scaled to -1.0 - +1.0) using Legendre Polynomials
      computeDistortionCorrections(xtScaled, 0.0, scaledDeltax);

      // compute residual distortion in unscaled focal plane coordinates
      computeResidualDistortionCorrections(xt, deltax2, deltay2);

      // update unscaled coordinates
      xt = -(uxScaled-scaledDeltax) * m_focalPlaneHalf_x - deltax2;
      yt = uy - deltay2;

      // check for convergenceyScaledDistortion
      if ((fabs(xt - xprevious) <= tolerance) && (fabs(yt - yprevious) <= tolerance)) {
        bConverged = true;
        break;
      }

      xprevious = xt;
      yprevious = yt;
    }

    if (bConverged) {
      // scale x coord inate back to detector (-32.5 to +32.5)
      // xtScaled *= -m_focalPlaneHalf_x;

      // set distorted coordinates
      p_focalPlaneX = xt;
      p_focalPlaneY = yt;
    }

    return bConverged;
  }


  /** Compute distortion corrections in x and y direction
   *
   * For Legendre Polynomials, see ...
   *
   * http://mathworld.wolfram.com/LegendrePolynomial.html
   * http://www.boost.org/doc/libs/1_36_0/libs/math/doc/sf_and_dist/html/math_toolkit/special/sf_poly/legendre.html
   *
   * @param xscaled focal plane x scaled to range of 1- to 1 for Legendre Polynomials
   * @param yscaled focal plane y scaled to range of 1- to 1 for Legendre Polynomials
   * @param deltax focal plane distortion correction to x in millimeters
   * @param deltay focal plane distortion correction to y in millimeters
   *
   * @return success/failure
   */
  bool NewHorizonsMvicTdiCameraDistortionMap::computeDistortionCorrections(
      const double xscaled,
      const double yscaled,
      double &deltax) {

    double lpx0, lpx1, lpx2, lpx3, lpx4, lpx5;
    double lpy0, lpy1, lpy2, lpy3, lpy4, lpy5;

    // Legendre polynomials
    try {
      lpx0 = legendre_p(0,xscaled);
      lpx1 = legendre_p(1,xscaled);
      lpx2 = legendre_p(2,xscaled);
      lpx3 = legendre_p(3,xscaled);
      lpx4 = legendre_p(4,xscaled);
      lpx5 = legendre_p(5,xscaled);
      lpy0 = legendre_p(0,yscaled);
      lpy1 = legendre_p(1,yscaled);
      lpy2 = legendre_p(2,yscaled);
      lpy3 = legendre_p(3,yscaled);
      lpy4 = legendre_p(4,yscaled);
      lpy5 = legendre_p(5,yscaled);
    }
    catch (const std::exception& e) {
      return false;
    }

    deltax =
       m_xDistortionCoeffs[0] * lpx0  * lpy1 +
       m_xDistortionCoeffs[1] * lpx1 * lpy0 +
       m_xDistortionCoeffs[2] * lpx0 * lpy2 +
       m_xDistortionCoeffs[3] * lpx1 * lpy1 +
       m_xDistortionCoeffs[4] * lpx2 * lpy0 +
       m_xDistortionCoeffs[5] * lpx0 * lpy3 +
       m_xDistortionCoeffs[6] * lpx1 * lpy2 +
       m_xDistortionCoeffs[7] * lpx2 * lpy1 +
       m_xDistortionCoeffs[8] * lpx3 * lpy0 +
       m_xDistortionCoeffs[9] * lpx0 * lpy4 +
      m_xDistortionCoeffs[10] * lpx1 * lpy3 +
      m_xDistortionCoeffs[11] * lpx2 * lpy2 +
      m_xDistortionCoeffs[12] * lpx3 * lpy1 +
      m_xDistortionCoeffs[13] * lpx4 * lpy0 +
      m_xDistortionCoeffs[14] * lpx0 * lpy5 +
      m_xDistortionCoeffs[15] * lpx1 * lpy4 +
      m_xDistortionCoeffs[16] * lpx2 * lpy3 +
      m_xDistortionCoeffs[17] * lpx3 * lpy2 +
      m_xDistortionCoeffs[18] * lpx4 * lpy1 +
      m_xDistortionCoeffs[19] * lpx5 * lpy0;

    return true;
  }


  /** Compute residual distortion corrections in row and column direction
   *  TODO: Implementati plete
   *
   * @param dx
   * @param dy
   * @param residualDeltax
   * @param residualDeltay
   *
   * @return success/failure
   */
  void NewHorizonsMvicTdiCameraDistortionMap::computeResidualDistortionCorrections(const double dx,
                                                             double &residualDeltax,
                                                             double &residualDeltay) {

    double s = 2500.5 - dx/0.013;

    residualDeltax = s * m_residualColDistCoeffs[1] +
                       pow(s,2) * m_residualColDistCoeffs[2] +
                       pow(s,3) * m_residualColDistCoeffs[3] +
                       pow(s,4) * m_residualColDistCoeffs[4] +
                       pow(s,5) * m_residualColDistCoeffs[5];

    residualDeltay = s * m_residualRowDistCoeffs[1] +
                       pow(s,2) * m_residualRowDistCoeffs[2] +
                       pow(s,3) * m_residualRowDistCoeffs[3] +
                       pow(s,4) * m_residualRowDistCoeffs[4] +
                       pow(s,5) * m_residualRowDistCoeffs[5];

    // convert to mm (correction for negative x to the left)
    residualDeltax *= -p_camera->PixelPitch();
    residualDeltay *= p_camera->PixelPitch();
  }


///**
// * Testing method to output corrections in x and y at pixel centers for entire focal plane.
// * Output in csv format for viewing/plotting in Excel.
// */
//bool NewHorizonsMvicTdiCameraDistortionMap::outputResidualDeltas() {
//
//  QString ofname("mvic_tdi_residual_deltas.csv");
//  std::ofstream fp_out(ofname.toLatin1().data(), std::ios::out);
//  if (!fp_out)
//    return false;
//
//  char buf[1056];
//
//  double residualColDelta;
//
//  for (int s=0; s <= 5000; s++) {  // loop in sample direction
//    residualColDelta =            m_residualColDistCoeffs[0] +
//                              s * m_residualColDistCoeffs[1] +
//                       pow(double(s),2) * m_residualColDistCoeffs[2] +
//                       pow(double(s),3) * m_residualColDistCoeffs[3] +
//                       pow(double(s),4) * m_residualColDistCoeffs[4] +
//                       pow(double(s),5) * m_residualColDistCoeffs[5];
//    sprintf(buf, "%d,%lf\n", s, residualColDelta);
//    fp_out << buf;
//  }
//
//  fp_out.close();
//
//  return true;
//}

}
