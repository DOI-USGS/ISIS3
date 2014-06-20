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

#include <QDebug>

#include <boost/math/special_functions/legendre.hpp>

#include "Constants.h"
#include "FunctionTools.h"
#include "IString.h"
#include "MvicCameraDistortionMap.h"

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
  MvicCameraDistortionMap::MvicCameraDistortionMap(Camera *parent) :
                           CameraDistortionMap(parent, 1.0) {
    m_numDistCoef = 20;
    m_distCoefX.resize(m_numDistCoef);
    m_distCoefY.resize(m_numDistCoef);
  }

  /** Destructor
   */
  MvicCameraDistortionMap::~MvicCameraDistortionMap() {
  }


  /**
   * @param naifIkCode 
   */
  void MvicCameraDistortionMap::SetDistortion(const int naifIkCode) {

    //read the distortion coefs from the NAIF Kernels
    QString naifXKey = "INS-98903_DISTORTION_COEF_X";
    QString naifYKey = "INS-98903_DISTORTION_COEF_Y";
    qDebug()<<"Before Loop";
    for (int i=0; i < m_numDistCoef; i++) {
      qDebug()<<"num coef = "<<m_numDistCoef;
      double d = p_camera->getDouble(naifXKey,i);;
      qDebug()<<"naifXKey = "<<naifXKey<<"     d = "<<d;
      m_distCoefX[i] = p_camera->getDouble(naifXKey,i);
      qDebug()<<"after X";
      m_distCoefY[i] = p_camera->getDouble(naifYKey,i);
      qDebug()<<"after Y";
    }
    qDebug()<<"After Loop";
    // Set boresight (typically referred to as the principal point offset by photogrammetrists -
    // what are photogrammetrist???). The boresights in the ik are based on -X boresight.  In Isis
    // boresight needs to be in Z.
    m_boreX = 0.0;
    m_boreY = 0.0;
  }


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
  bool MvicCameraDistortionMap::SetFocalPlane(const double dx, const double dy) {
/*
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // scale dx and dy to lie between -1 and 1 (requirement for Legendre Polynomials, man)
    double dx_scaled = dx - 65.312/2;
    double dy_scaled = dy - 1.664/2;

    double lpx0, lpx1, lpx2, lpx3, lpx4, lpx5;
    double lpy0, lpy1, lpy2, lpy3, lpy4, lpy5;

    lpx0 = boost::math::legendre_p(0,dx_scaled);
    lpx1 = boost::math::legendre_p(1,dx_scaled);
    lpx2 = boost::math::legendre_p(2,dx_scaled);
    lpx3 = boost::math::legendre_p(3,dx_scaled);
    lpx4 = boost::math::legendre_p(4,dx_scaled);
    lpx5 = boost::math::legendre_p(5,dx_scaled);
    lpy0 = boost::math::legendre_p(0,dy_scaled);
    lpy1 = boost::math::legendre_p(1,dy_scaled);
    lpy2 = boost::math::legendre_p(2,dy_scaled);
    lpy3 = boost::math::legendre_p(3,dy_scaled);
    lpy4 = boost::math::legendre_p(4,dy_scaled);
    lpy5 = boost::math::legendre_p(5,dy_scaled);

//  double deltax =
//  double deltay =

    p_undistortedFocalPlaneX =  m_boreX +
        m_distCoefX[0] +  dx*m_distCoefX[1] + dx*(m_distCoefX[2]*m_distCoefX[2]) +
        dx*(m_distCoefX[3] * m_distCoefX[3] * m_distCoefX[3]);
    p_undistortedFocalPlaneY =  m_boreY +
        m_distCoefY[0] +  dy*m_distCoefY[1] + dy*(m_distCoefY[2]*m_distCoefY[2]) +
        dy*(m_distCoefY[3] * m_distCoefY[3] * m_distCoefY[3]);
//  p_undistortedFocalPlaneY =  m_boreY +
//    m_distCoefY[0] + dy*(m_distCoefY[1] + dy*(m_distCoefY[2] + dy*m_distCoefY[3])) + dy;

      qDebug() << "distorted: " << dx << " " << dy;
      qDebug() << "undistorted: " << p_undistortedFocalPlaneX << " " << p_undistortedFocalPlaneY;
*/
    return true;
  }


  /** Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   *
   * @return if the conversion was successful
   * @see SetDistortion
   */
  bool MvicCameraDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
/*
    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    //std::cout << "Distortionless : " << p_undistortedFocalPlaneX << " " << p_undistortedFocalPlaneY << "\n";

    //cubic distortion model
    //use the cubic equation to find the distorted Y value
    double a,b,c; //coefficients of the cubic (coef x^3 ==1)

    a = m_distCoefY[2] / m_distCoefY[3];
    b = (1.0 + m_distCoefY[1]) / m_distCoefY[3];
    c = (m_distCoefY[0] + m_boreY - uy) / m_distCoefY[3];

    QList<double> roots = FunctionTools::realCubicRoots(1.0,a,b,c);

    if (roots.size() == 1) { //one root
      p_focalPlaneY = roots[0];
    }
    else {  //pick the root closest to the orginal value
      QList<double> delta;

      //store all the distance from undistored to focal plane
      for (int i=0;i<roots.size();i++)
        delta << fabs(roots[i]-uy);

      if ( roots.size() == 3) //three roots to choose among
        p_focalPlaneY = delta[0] < delta[1] ? 
                       (delta[0] < delta[2] ? roots[0]:roots[2]) : 
                       (delta[1] < delta[2] ? roots[1]:roots[2]) ;
      else  //two roots to choose between
        p_focalPlaneY = delta[0] < delta[1] ? roots[0]:roots[1]  ;
    }
     
    //now that we know the distortedY we can directly calculate the X
    p_focalPlaneX = ux - (m_boreX +m_distCoefX[0] + 
                    p_focalPlaneY*(m_distCoefX[1] + 
                    p_focalPlaneY*(m_distCoefX[2] +  
                    p_focalPlaneY* m_distCoefX[3])));
*/
    return true;
  }
}
