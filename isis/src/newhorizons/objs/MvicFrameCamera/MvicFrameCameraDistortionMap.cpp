/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <cmath>
#include <iostream>

#include <boost/math/special_functions/legendre.hpp>

#include "Camera.h"
#include "Constants.h"
#include "FunctionTools.h"
#include "IString.h"
#include "MvicFrameCameraDistortionMap.h"

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
   * @param parent        the parent camera that will use this distortion map
   * @param zDirection    the direction of the focal plane Z-axis
   *                      (either 1 or -1)
   *
   */
  MvicFrameCameraDistortionMap::MvicFrameCameraDistortionMap(Camera *parent,
                                                             vector<double> xdistcoeffs,
                                                             vector<double> ydistcoeffs,
                                                             double borex, double borey) :
    CameraDistortionMap(parent, 1.0) {

    m_numDistCoef = 20;
    m_distCoefX = xdistcoeffs;
    m_distCoefY = ydistcoeffs;

    m_boreX = borex;
    m_boreY = borey;

    m_detectorHalf_x = 0.5 * p_camera->Samples() * p_camera->PixelPitch(); // 32.656 mm
    m_detectorHalf_y = 0.5 * p_camera->Lines() * p_camera->PixelPitch();   //  0.832 mm
  }

  /** Destructor
   */
  MvicFrameCameraDistortionMap::~MvicFrameCameraDistortionMap() {
  }


  /**
   * Testing method to output corrections in x and y at pixel centers for entire focal plane.
   * Output in csv format for viewing/plotting in Excel.
   */
/*  bool MvicFrameCameraDistortionMap::outputdeltas() {

    QString ofname("mvic_frame_deltas.csv");
    std::ofstream fp_out(ofname.toAscii().data(), std::ios::out);
    if (!fp_out)
      return false;

    char buf[1056];

    for (double y = -0.832; y <= 1.664; y += 0.013) {    // loop in y direction
      for (double x=-32.656; x <= 32.656; x += 0.013) {  // loop in x direction
        SetFocalPlane(x,y);
        sprintf(buf, "%lf,%lf,%lf,%lf\n", x, m_dx, y, m_dy);
        fp_out << buf;
      }
    }

    fp_out.close();

    return true;
  }
*/


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
  bool MvicFrameCameraDistortionMap::SetFocalPlane(const double dx, const double dy) {

    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // scale x and y to lie in the range -1.0 to +1.0
    // this is requirement for Legendre Polynomials, man
    double xscaled = dx/m_detectorHalf_x;
    double yscaled = dy/m_detectorHalf_y;

    // compute distortion corrections in x and y using Legendre Polynomials
    // these corrections are also in the -1.0 to +1.0 range
    double deltax, deltay;
    computeDistortionCorrections(xscaled, yscaled, deltax, deltay);

    // apply the corrections
    xscaled += deltax;
    yscaled += deltay;

    // scale back from range of '-1.0 to +1.0' to the detector, '-32.656 to 32.656 mm'
    p_undistortedFocalPlaneX = xscaled * m_detectorHalf_x;
    p_undistortedFocalPlaneY = yscaled * m_detectorHalf_y;

    return true;
  }


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
  bool MvicFrameCameraDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {

    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    double xScaledDistortion, yScaledDistortion;

    // scale undistorted coordinates to range of -1.0 to +1.0
    double xtScaled = ux/m_detectorHalf_x;
    double ytScaled = uy/m_detectorHalf_y;

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
      // scale coordinates back to detector (-32.656 to +32.656)
      xtScaled *= m_detectorHalf_x;
      ytScaled *= m_detectorHalf_y;

      // set distorted coordinates
      p_focalPlaneX = xtScaled;
      p_focalPlaneY = ytScaled;
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
   * @return if successful
   */
  bool MvicFrameCameraDistortionMap::computeDistortionCorrections(const double xscaled,
                                                             const double yscaled, double &deltax,
                                                             double &deltay) {

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
    catch (const std::exception& e) {
      return false;
    }

    deltax =
       m_distCoefX[0] * lpx0 * lpy1 +
       m_distCoefX[1] * lpx1 * lpy0 +
       m_distCoefX[2] * lpx0 * lpy2 +
       m_distCoefX[3] * lpx1 * lpy1 +
       m_distCoefX[4] * lpx2 * lpy0 +
       m_distCoefX[5] * lpx0 * lpy3 +
       m_distCoefX[6] * lpx1 * lpy2 +
       m_distCoefX[7] * lpx2 * lpy1 +
       m_distCoefX[8] * lpx3 * lpy0 +
       m_distCoefX[9] * lpx0 * lpy4 +
      m_distCoefX[10] * lpx1 * lpy3 +
      m_distCoefX[11] * lpx2 * lpy2 +
      m_distCoefX[12] * lpx3 * lpy1 +
      m_distCoefX[13] * lpx4 * lpy0 +
      m_distCoefX[14] * lpx0 * lpy5 +
      m_distCoefX[15] * lpx1 * lpy4 +
      m_distCoefX[16] * lpx2 * lpy3 +
      m_distCoefX[17] * lpx3 * lpy2 +
      m_distCoefX[18] * lpx4 * lpy1 +
      m_distCoefX[19] * lpx5 * lpy0;

    deltay =
      m_distCoefY[0] * lpx0 * lpy1 +
      m_distCoefY[1] * lpx1 * lpy0 +
      m_distCoefY[2] * lpx0 * lpy2 +
      m_distCoefY[3] * lpx1 * lpy1 +
      m_distCoefY[4] * lpx2 * lpy0 +
      m_distCoefY[5] * lpx0 * lpy3 +
      m_distCoefY[6] * lpx1 * lpy2 +
      m_distCoefY[7] * lpx2 * lpy1 +
      m_distCoefY[8] * lpx3 * lpy0 +
      m_distCoefY[9] * lpx0 * lpy4 +
     m_distCoefY[10] * lpx1 * lpy3 +
     m_distCoefY[11] * lpx2 * lpy2 +
     m_distCoefY[12] * lpx3 * lpy1 +
     m_distCoefY[13] * lpx4 * lpy0 +
     m_distCoefY[14] * lpx0 * lpy5 +
     m_distCoefY[15] * lpx1 * lpy4 +
     m_distCoefY[16] * lpx2 * lpy3 +
     m_distCoefY[17] * lpx3 * lpy2 +
     m_distCoefY[18] * lpx4 * lpy1 +
     m_distCoefY[19] * lpx5 * lpy0;

    return true;
  }
}
