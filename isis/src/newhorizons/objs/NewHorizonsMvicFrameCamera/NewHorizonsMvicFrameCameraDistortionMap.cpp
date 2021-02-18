/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>
#include <iostream>

#include <boost/math/special_functions/legendre.hpp>

#include "Camera.h"
#include "Constants.h"
#include "FunctionTools.h"
#include "IString.h"
#include "NewHorizonsMvicFrameCameraDistortionMap.h"

#include "CameraFocalPlaneMap.h"


#include <QDebug>


using namespace boost::math;
using namespace std;
using namespace Isis;

namespace Isis {
  /** Camera distortion map constructor
   *
   * This class maps between distorted and undistorted focal plane x/y's. The default mapping is the
   * identity, that is, the focal plane x/y and undistorted focal plane x/y will be identical.
   *
   * @param parent              the parent camera that will use this distortion map
   * @param zDirection          the direction of the focal plane Z-axis
   *                            (either 1 or -1)
   *
   * @param xDistortionCoeffs   distortion coefficients in x
   * @param yDistortionCoeffs   distortion coefficients in y
   */
  NewHorizonsMvicFrameCameraDistortionMap::NewHorizonsMvicFrameCameraDistortionMap(Camera *parent,
                                                             vector<double> xDistortionCoeffs,
                                                             vector<double> yDistortionCoeffs) :
    CameraDistortionMap(parent, 1.0) {

    m_xDistortionCoeffs = xDistortionCoeffs;
    m_yDistortionCoeffs = yDistortionCoeffs;

    double pixelPitch = p_camera->PixelPitch();

    m_focalPlaneHalf_x = 0.5 * p_camera->Samples() * pixelPitch; // 32.5 mm
    m_focalPlaneHalf_y = 0.5 * p_camera->Lines() * pixelPitch;   // 0.832 mm
  }


  /** Destructor
   */
  NewHorizonsMvicFrameCameraDistortionMap::~NewHorizonsMvicFrameCameraDistortionMap() {
  }


//  /**
//   * Testing method to output corrections in x and y at pixel centers for entire focal plane.
//   * Output in csv format for viewing/plotting in Excel.
//   */
//bool NewHorizonsMvicFrameCameraDistortionMap::outputDeltas() {
//
//  QString ofname("mvic_frame_deltas.csv");
//  std::ofstream fp_out(ofname.toLatin1().data(), std::ios::out);
//  if (!fp_out)
//    return false;
//
//  char buf[1056];
//
//  double deltax, deltay;
//
//  for (double line = 0.5; line <= 128.5; line += 1.0) {    // loop in y direction
//    for (double sample=0.5; sample <= 5000.5; sample += 1.0) {      // loop in x direction
//
//      p_camera->FocalPlaneMap()->SetDetector(sample,line);
//
//      double fplanex = p_camera->FocalPlaneMap()->FocalPlaneX();
//      double fplaney = p_camera->FocalPlaneMap()->FocalPlaneY();
//
//      SetFocalPlane(fplanex,fplaney);
//
//      deltax = fplanex - p_undistortedFocalPlaneX;
//      deltay = fplaney - p_undistortedFocalPlaneY;
//
//      sprintf(buf, "%lf,%lf,%lf,%lf\n", sample, deltax/0.013, line, deltay/0.013);
//
//      fp_out << buf;
//    }
//  }
//
//  fp_out.close();
//
//  return true;
//}


  /** Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x/y given a distorted focal plane x/y.
   *
   * @param dx distorted focal plane x in millimeters
   * @param dy distorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   */
  bool NewHorizonsMvicFrameCameraDistortionMap::SetFocalPlane(const double dx, const double dy) {

    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // in case of failures, initialize undistorted focal plane coordinates to the distorted
    // coordinate values
    p_undistortedFocalPlaneX = dx;
    p_undistortedFocalPlaneY = dy;

    // if x and/or y lie outside of the detector, do NOT apply distortion
    // set undistorted focal plane values to be identical to raw values
    if ((fabs(dx) > m_focalPlaneHalf_x) || (fabs(dy) > m_focalPlaneHalf_y)) {
      return true;
    }

    // shift from ISIS MVIC FT image coordinate system with +x to the left and +y down to
    // the desired system of +x to the right and +y up
    // e.g. negate x and y

    // scale x and y to lie in the range -1.0 to +1.0
    // this is requirement for Legendre Polynomials, man
    double xscaled = -dx/m_focalPlaneHalf_x;
    double yscaled = -dy/m_focalPlaneHalf_y;

    // compute distortion corrections in x and y using Legendre Polynomials
    // these corrections are also in the -1.0 to +1.0 range
    // if Legendre computations fail, we set undistorted focal plane x and y
    // to be identical to distorted x and y and return true
    double deltax, deltay;
    if (!computeDistortionCorrections(xscaled, yscaled, deltax, deltay)) {
      return true;
    }

    // apply the corrections to original x,y
    xscaled += deltax;
    yscaled += deltay;

    // scale back from range of '-1.0 to +1.0' to the detector, '-32.5 to 32.5 mm'
    p_undistortedFocalPlaneX = -xscaled * m_focalPlaneHalf_x;
    p_undistortedFocalPlaneY = -yscaled * m_focalPlaneHalf_y;

    return true;
  }


//  bool NewHorizonsMvicFrameCameraDistortionMap::SetFocalPlane(const double dx, const double dy) {

//    p_focalPlaneX = dx;
//    p_focalPlaneY = dy;

//    // if x and/or y lie outside of the detector, do NOT apply distortion
//    // set undistorted focal plane values to be identical to raw values
//    if ((fabs(dx) > m_focalPlaneHalf_x) || (fabs(dy) > m_focalPlaneHalf_y)) {
//      p_undistortedFocalPlaneX = dx;
//      p_undistortedFocalPlaneY = dy;

//      return true;
//    }

//    // scale x and y to lie in the range -1.0 to +1.0
//    // this is requirement for Legendre Polynomials, man
//    double xscaled = dx/m_focalPlaneHalf_x;
//    double yscaled = dy/m_focalPlaneHalf_y;

//    // compute distortion corrections in x and y using Legendre Polynomials
//    // these corrections are also in the -1.0 to +1.0 range
//    double deltax, deltay;
//    computeDistortionCorrections(xscaled, yscaled, deltax, deltay);

//    // apply the corrections
//    xscaled += deltax;
//    yscaled += deltay;

//    // scale back from range of '-1.0 to +1.0' to the detector, '-32.656 to 32.656 mm'
//    p_undistortedFocalPlaneX = xscaled * m_focalPlaneHalf_x;
//    p_undistortedFocalPlaneY = yscaled * m_focalPlaneHalf_y;

//    return true;
//  }


  /** Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   *
   * This is an iterative procedure.
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   */
  bool NewHorizonsMvicFrameCameraDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {

    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    double xScaledDistortion, yScaledDistortion;

    // scale undistorted coordinates to range of -1.0 to +1.0
    double xtScaled = -ux/m_focalPlaneHalf_x;
    double ytScaled = -uy/m_focalPlaneHalf_y;

    double uxScaled = xtScaled;
    double uyScaled = ytScaled;

    double xScaledPrevious = 1000000.0;
    double yScaledPrevious = 1000000.0;

    double tolerance = 0.000001;

    bool bConverged = false;

    // iterating to introduce distortion...
    // we stop when the difference between distorted coordinates
    // in successive iterations is at or below the given tolerance
    for( int i = 0; i < 50; i++ ) {

      // compute distortion in x and y (scaled to -1.0 - +1.0) using Legendre Polynomials
      computeDistortionCorrections(xtScaled, ytScaled, xScaledDistortion, yScaledDistortion);

      // update scaled image coordinates
      xtScaled = uxScaled - xScaledDistortion;
      ytScaled = uyScaled - yScaledDistortion;

      // check for convergence
      if((fabs(xtScaled - xScaledPrevious) <= tolerance) && (fabs(ytScaled - yScaledPrevious) <= tolerance)) {
        bConverged = true;
        break;
      }

      xScaledPrevious = xtScaled;
      yScaledPrevious = ytScaled;
    }

    if (bConverged) {
      // scale coordinates back to detector (-32.5 to +32.5)
      xtScaled *= -m_focalPlaneHalf_x;
      ytScaled *= -m_focalPlaneHalf_y;

      // set distorted coordinates
      p_focalPlaneX = xtScaled;
      p_focalPlaneY = ytScaled;
    }

    return bConverged;
  }
//  bool NewHorizonsMvicFrameCameraDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {

//    // image coordinates prior to introducing distortion
//    p_undistortedFocalPlaneX = ux;
//    p_undistortedFocalPlaneY = uy;

//    double xScaledDistortion, yScaledDistortion;

//    // scale undistorted coordinates to range of -1.0 to +1.0
//    double xtScaled = ux/m_focalPlaneHalf_x;
//    double ytScaled = uy/m_focalPlaneHalf_y;

//    double uxScaled = xtScaled;
//    double uyScaled = ytScaled;

//    double xScaledPrevious = 1000000.0;
//    double yScaledPrevious = 1000000.0;

//    double tolerance = 0.000001;

//    bool bConverged = false;

//    // iterating to introduce distortion...
//    // we stop when the difference between distorted coordinates
//    // in successive iterations is at or below the given tolerance
//    for( int i = 0; i < 50; i++ ) {

//      // compute distortion in x and y (scaled to -1.0 - +1.0) using Legendre Polynomials
//      computeDistortionCorrections(xtScaled, ytScaled, xScaledDistortion, yScaledDistortion);

//      // update scaled image coordinates
//      xtScaled = uxScaled - xScaledDistortion;
//      ytScaled = uyScaled - yScaledDistortion;

//      // check for convergence
//      if((fabs(xtScaled - xScaledPrevious) <= tolerance) && (fabs(ytScaled - yScaledPrevious) <= tolerance)) {
//        bConverged = true;
//        break;
//      }

//      xScaledPrevious = xtScaled;
//      yScaledPrevious = ytScaled;
//    }

//    if (bConverged) {
//      // scale coordinates back to detector (-32.656 to +32.656)
//      xtScaled *= m_focalPlaneHalf_x;
//      ytScaled *= m_focalPlaneHalf_y;

//      // set distorted coordinates
//      p_focalPlaneX = xtScaled;
//      p_focalPlaneY = ytScaled;
//    }

//    return bConverged;
//  }


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
   * @return if successful
   */
  bool NewHorizonsMvicFrameCameraDistortionMap::computeDistortionCorrections(const double xscaled,
                                                                  const double yscaled,
                                                                  double &deltax, double &deltay) {

    double lpx0, lpx1, lpx2, lpx3, lpx4, lpx5;
    double lpy0, lpy1, lpy2, lpy3, lpy4, lpy5;

    // Legendre polynomials
    // boost library method legendre_p will generate an exception if xscaled or yscaled do not lie
    // between -1 to 1 (inclusive). In this event we return false.
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
    // TESTING NOTE: Could not find a way to cause this error. If one is found a test should be added
    catch (const std::exception& e) {
      return false;
    }

    deltax =
       m_xDistortionCoeffs[0] * lpx0 * lpy1 +
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

    deltay =
      m_yDistortionCoeffs[0] * lpx0 * lpy1 +
      m_yDistortionCoeffs[1] * lpx1 * lpy0 +
      m_yDistortionCoeffs[2] * lpx0 * lpy2 +
      m_yDistortionCoeffs[3] * lpx1 * lpy1 +
      m_yDistortionCoeffs[4] * lpx2 * lpy0 +
      m_yDistortionCoeffs[5] * lpx0 * lpy3 +
      m_yDistortionCoeffs[6] * lpx1 * lpy2 +
      m_yDistortionCoeffs[7] * lpx2 * lpy1 +
      m_yDistortionCoeffs[8] * lpx3 * lpy0 +
      m_yDistortionCoeffs[9] * lpx0 * lpy4 +
     m_yDistortionCoeffs[10] * lpx1 * lpy3 +
     m_yDistortionCoeffs[11] * lpx2 * lpy2 +
     m_yDistortionCoeffs[12] * lpx3 * lpy1 +
     m_yDistortionCoeffs[13] * lpx4 * lpy0 +
     m_yDistortionCoeffs[14] * lpx0 * lpy5 +
     m_yDistortionCoeffs[15] * lpx1 * lpy4 +
     m_yDistortionCoeffs[16] * lpx2 * lpy3 +
     m_yDistortionCoeffs[17] * lpx3 * lpy2 +
     m_yDistortionCoeffs[18] * lpx4 * lpy1 +
     m_yDistortionCoeffs[19] * lpx5 * lpy0;

    return true;
  }
}
