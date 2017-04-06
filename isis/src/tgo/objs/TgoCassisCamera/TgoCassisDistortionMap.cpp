/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/11/24 16:40:30 $
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
#include <cmath>

#include <QDebug>

#include "IString.h"
#include "TgoCassisDistortionMap.h"

namespace Isis {
  /** 
   * Exomars TGO CaSSIS distortion map constructor.
   *
   * Create a camera distortion map.  This class maps between distorted
   * and undistorted focal plane x/y's. The default mapping is the
   * identity, that is, the focal plane x/y and undistorted focal plane
   * x/y will be identical.
   *
   * @param parent        the parent camera that will use this distortion map
   * @param zDirection    the direction of the focal plane Z-axis
   *                      (either 1 or -1)
   *
   */
  TgoCassisDistortionMap::TgoCassisDistortionMap(Camera *parent, 
                                                 int naifIkCode) 
      : CameraDistortionMap(parent) {

    m_width = 2048;
    m_height = 2048;

    QString od = "INS" + toString(naifIkCode) + "_OD_";

    for(int i = 0; i < 6; i++) {
      m_A1.push_back(p_camera->getDouble(od + "A1", i));
      m_A2.push_back(p_camera->getDouble(od + "A2", i));
      m_A3.push_back(p_camera->getDouble(od + "A3", i));
    }
  }


  /** 
   * Exomars TGO CaSSIS distortion map destructor.
   */
  TgoCassisDistortionMap::~TgoCassisDistortionMap() {
  }


  /** 
   * Compute undistorted focal plane (x,y) coordinate from the distorted (x,y).
   *
   * @param dx distorted focal plane x, in millimeters
   * @param dy distorted focal plane y, in millimeters
   *
   * @return @b bool Indicates whether the conversion was successful.
   */
  bool TgoCassisDistortionMap::SetFocalPlane(const double dx, 
                                             const double dy) {

    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // i and j are normalized distorted coordinates
    double i = normalize(dx, m_width,  m_height);
    double j = normalize(dy, m_height, m_width);

    // convenience variables
    double i2 = i*i;
    double j2 = j*j;
    double ij = i*j;

    // divider we might nead for debuging
    double divider = i2 * m_A3[0] 
                     + ij * m_A3[1] 
                     + j2 * m_A3[2] 
                     + i  * m_A3[3] 
                     + j  * m_A3[4] 
                     + 1;

    double xNorm = ( i2 * m_A1[0] 
                     + ij * m_A1[1] 
                     + j2 * m_A1[2] 
                     + i  * m_A1[3] 
                     + j  * m_A1[4] 
                     + m_A1[5] )
                   / divider;

    double yNorm = ( i2 * m_A2[0] 
                     + ij * m_A2[1] 
                     + j2 * m_A2[2] 
                     + i  * m_A2[3] 
                     + j  * m_A2[4] 
                     + m_A2[5] )
                   / divider;

    // denormalize ideal (x,y) coordinates
    p_undistortedFocalPlaneX = denormalize(xNorm, m_width,  m_height);
    p_undistortedFocalPlaneY = denormalize(yNorm, m_height, m_width);

    return true;
  }


  /** 
   * Compute distorted focal plane (x,y) given an undistorted focal plane (x,y).
   *
   * @param ux undistorted focal plane x, in millimeters
   * @param uy undistorted focal plane y, in millimeters
   *
   * @return @b bool Indicates whether the conversion was successful
   */
  bool TgoCassisDistortionMap::SetUndistortedFocalPlane(const double ux,
                                                        const double uy) {
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    // x, y are normalized undistorted (ideal) coordinates
    double xNorm = normalize(ux, m_width,  m_height);
    double yNorm = normalize(uy, m_height, m_width);

    // i, j are distorted coordinates
    double iNorm = xNorm;
    double jNorm = yNorm;
    int newtonIterations = 2000;
    // newton's method to iterate
    for( int index = 0; index < newtonIterations; ++index ) {
      /* 
       * compute F(i_pre, j_pre)
       * 
       * F_vec(i,j) = [ initialX - ( A1 * chi' ) / ( A3 * chi' ) ]
       *              [ initialY - ( A2 * chi' ) / ( A3 * chi' ) ]
      */
      m_width = 0.0; 
      m_height = 0.0; // Below we call SetFocalPlane(). This method normalizes its inputs, i.e.
                      // iNorm and jNorm in this case. Since they are already normalized, we 
                      // set height = weight = 0.0 so that normalize(value) just
                      // returns value.

      if (SetFocalPlane(iNorm, jNorm) ) {
        double xPredict = p_undistortedFocalPlaneX;
        double yPredict = p_undistortedFocalPlaneY;
        double divider = iNorm * iNorm * m_A3[0] 
                         + iNorm * jNorm * m_A3[1] 
                         + jNorm * jNorm * m_A3[2] 
                         + iNorm * m_A3[3] 
                         + jNorm * m_A3[4] 
                         + 1;

        double f11 = xNorm - xPredict;
        double f21 = yNorm - yPredict;

        /* 
         * compute the Jacobian, J(i, j):
         * 
         * J(i,j) = [ - ( (A1 * dchi / di') - xPredicted*(A3 * dchi / di) ) / (A3 * chi'),...
         *            - ( (A1 * dchi / dj') - xPredicted*(A3 * dchi / dj) ) / (A3 * chi');...
         *            - ( (A2 * dchi / di') - yPredicted*(A3 * dchi / di) ) / (A3 * chi'),...
         *            - ( (A2 * dchi / dj') - yPredicted*(A3 * dchi / dj) ) / (A3 * chi')]
         * 
         * dchi / di = [2i   j   0   1   0   0]
         * dchi / dj = [ 0   i  2j   0   1   0]
         */
        double j11 = - ( ( 2 * iNorm * m_A1[0] + jNorm * m_A1[1] + m_A1[3] ) 
                         - xPredict * ( 2 * iNorm * m_A3[0] + jNorm * m_A3[1] + m_A3[3] ) ) 
                       / divider;
        double j12 = - ( ( iNorm * m_A1[1] + 2 * jNorm * m_A1[2] + m_A1[4] ) 
                         - xPredict * ( iNorm * m_A3[1] + 2 * jNorm * m_A3[2] + m_A3[4] ) )
                       / divider;
        double j21 = - ( ( 2 * iNorm * m_A2[0] + jNorm * m_A2[1] + m_A2[3] ) 
                         - yPredict * ( 2 * iNorm * m_A3[0] + jNorm * m_A3[1] + m_A3[3] ) )
                       / divider;
        double j22 = - ( ( iNorm * m_A2[1] + 2 * jNorm * m_A2[2] + m_A2[4] ) 
                         - yPredict * ( iNorm * m_A3[1] + 2 * jNorm * m_A3[2] + m_A3[4] ) )
                       / divider;

        /* 
         * compute update
         * 
         * [i_n, j_n]  = [i_n-1, j_n-1] - inv(J(i_n-1, j_n-1))*F(i_n-1, j_n-1);
         * 
         *                    1             [  J22 -J12 ] [ F11 ]   [  J22*F11 - J12*F21 ]
         * inv(J)*F =  ------------------ * [ -J21  J11 ] [ F21 ] = [ -J21*F11 + J11*F21 ]
         *             J11*J22 - J12*J21
         */
        double di = - ( j22*f11 - j12*f21) / (j11*j22 - j12*j21);
        double dj = - (-j21*f11 + j11*f21) / (j11*j22 - j12*j21);
        iNorm = iNorm + di;
        jNorm = jNorm + dj;
      }
      else {
        return false;
      }

    }
    m_width = 2048;
    m_height = 2048;

    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;
    // denormalize distorted (i,j) coordinates
    p_focalPlaneX = denormalize(iNorm, m_width,  m_height);
    p_focalPlaneY = denormalize(jNorm, m_height, m_width);
    return true;
  }


  /**
   * Normalize the value using the given scalars. This method uses the formula 
   * below to normalize the given value: 
   *  
   *  @f[ norm = \frac{ value - \frac{a}{2} }{ a + b } @f]
   * 
   * @param value Value to be normalize.
   * @param a Primary scaling value.
   * @param b Secondary scaling value.
   * 
   * @return @b double The normalized value.
   */
  double TgoCassisDistortionMap::normalize(double value, double a, double b) {
    double sum = a + b;
    if (sum == 0) {
      return value;
    }
    return (value - a / 2) / sum;
  }


  /**
   * De-normalize the value using the given scalars. This method is the 
   * inverse function of the normalize() function. It uses the formula below 
   * to denormalize the given value: 
   *  
   *  @f[ denorm = value * (a + b) + \frac{a}{2} @f]
   * 
   * @param value Value to be normalize.
   * @param a Primary scaling value.
   * @param b Secondary scaling value.
   * 
   * @return @b double The normalized value.
   */
  double TgoCassisDistortionMap::denormalize(double value, double a, double b) {
    double sum = a + b;
    if (sum == 0) {
      return value;
    }
    return value * sum + a / 2;
  }
  
}

