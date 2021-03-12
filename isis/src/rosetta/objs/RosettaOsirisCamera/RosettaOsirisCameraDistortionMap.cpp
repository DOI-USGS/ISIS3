/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "RosettaOsirisCameraDistortionMap.h"

using namespace std;

namespace Isis {
  /**
   * Create a camera distortion map.  This class maps between distorted
   * and undistorted focal plane x/y's.  The default mapping is the
   * identity, that is, the focal plane x/y and undistorted focal plane
   * x/y will be identical.
   *
   * @param parent the parent camera that will use this distortion map
   *
   */
  RosettaOsirisCameraDistortionMap::RosettaOsirisCameraDistortionMap(Camera *parent) :
                                                                   CameraDistortionMap(parent) {
    // Initialize to the identity transform
    m_toUnDistortedX = LinearAlgebra::zeroMatrix(4, 4);
    m_toUnDistortedY = LinearAlgebra::zeroMatrix(4, 4);
    m_toUnDistortedX(0, 1) = 1.0;
    m_toUnDistortedY(1, 0) = 1.0;

    m_boresightSample = 0.0;
    m_boresightLine = 0.0;
    m_pixelPitch = 1.0;
  }

  /**
   * Compute undistorted focal plane x/y given a distorted focal plane x/y.
   *
   * The distortion is modeled by pixelspace polynomials. The polynomials use
   * zero-based pixel space with the origin at the top left corner of the
   * image, so the input focal plane coordinates are converted to pixel
   * coordinates using the boresight location and pixel pitch. After
   * computation, they are converted back into focal plane coordinates by the
   * inverse process.
   *
   * Given a set of distorted pixel coordinates (dx, dy), the undistorted pixel
   * coordinates (ux, uy) are computed as:
   * @f[ (ux, uy) = F(dx, dy) = ( \sum_{i=0}^3 \sum_{j=0}^3 C_{i,j}^x dx^i dy^j,
   * \sum_{i=0}^3 \sum_{j=0}^3 C_{i,j}^y dx^i dy^j) @f] where @f$ C_{i,j}^y @f$
   * and @f$ C_{i,j}^y @f$ are the @f$ (i,j)^{\text{th}} @f$ coefficients of
   * the @f$ x @f$ and @f$ y @f$ polynomials respectively.
   *
   * This calculation is actually performed as follows:
   * @f[ (ux, uy) =
   * \left( C^x \begin{bmatrix} 1 \\ dx \\ dx^2 \\ dx^3 \end{bmatrix}
   *        \cdot \begin{bmatrix} 1 \\ dy \\ dy^2 \\ dy^3 \end{bmatrix},
   *        C^y \begin{bmatrix} 1 \\ dx \\ dx^2 \\ dx^3 \end{bmatrix}
   *        \cdot \begin{bmatrix} 1 \\ dy \\ dy^2 \\ dy^3 \end{bmatrix} \right)
   * @f]
   * where @f$ C^x @f$ and @f$ C^y @f$ are the @f$ x @f$ and @f$ y @f$
   * coefficient matrices respectively.
   *
   * @param dx distorted focal plane x in millimeters
   * @param dy distorted focal plane y in millimeters
   *
   * @return @b bool if the conversion was successful
   */
  bool RosettaOsirisCameraDistortionMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    // The equations are in pixel coordinates so convert
    double dxPixel = focalXToLine(dx);
    double dyPixel = focalYToSample(dy);

    LinearAlgebra::Vector xTerms = LinearAlgebra::zeroVector(4);
    xTerms(0) = 1.0;
    xTerms(1) = dxPixel;
    xTerms(2) = dxPixel*dxPixel;
    xTerms(3) = dxPixel*dxPixel*dxPixel;

    LinearAlgebra::Vector yTerms = LinearAlgebra::zeroVector(4);
    yTerms(0) = 1.0;
    yTerms(1) = dyPixel;
    yTerms(2) = dyPixel*dyPixel;
    yTerms(3) = dyPixel*dyPixel*dyPixel;

    double ux = LinearAlgebra::dotProduct(
                          LinearAlgebra::multiply(m_toUnDistortedX, xTerms),
                          yTerms);
    double uy = LinearAlgebra::dotProduct(
                          LinearAlgebra::multiply(m_toUnDistortedY, xTerms),
                          yTerms);

    p_undistortedFocalPlaneX = lineToFocalX(ux);
    p_undistortedFocalPlaneY = sampleToFocalY(uy);

    return true;
  }

  /**
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   *
   * The conversion is performed using Newton's method to find distorted
   * coordinates whose undistorted coordinates are within @f$ 10^{-7} @f$
   * pixels of the input undistorted coordinates. The input undistorted
   * coordinates are used as an initial guess for the distorted coordinates.
   *
   * Given a set of undistorted pixel coordinates (ux, uy), the object function
   * is:
   * @f[
   * G(dx, dy) = (ux, uy) - F(dx, dy)
   * @f]
   * where @f$ F @f$ is the transformation from distorted to undistorted pixel
   * coordinates.
   *
   * Then, the negative jacobian is:
   * @f[
   * -J_G(dx, dy) = \begin{bmatrix}
   *   C^x \begin{bmatrix} 0 \\ 1 \\ 2dx \\ 3dx^2 \end{bmatrix}
   *   \cdot \begin{bmatrix} 1 \\ dy \\ dy^2 \\ dy^3 \end{bmatrix} &
   *   C^x \begin{bmatrix} 1 \\ dx \\ dx^2 \\ dx^3 \end{bmatrix}
   *   \cdot \begin{bmatrix} 0 \\ 1 \\ 2dy \\ 3dy^2 \end{bmatrix} \\
   *   C^y \begin{bmatrix} 0 \\ 1 \\ 2dx \\ 3dx^2 \end{bmatrix}
   *   \cdot \begin{bmatrix} 1 \\ dy \\ dy^2 \\ dy^3 \end{bmatrix} &
   *   C^y \begin{bmatrix} 1 \\ dx \\ dx^2 \\ dx^3 \end{bmatrix}
   *   \cdot \begin{bmatrix} 0 \\ 1 \\ 2dy \\ 3dy^2 \end{bmatrix} \end{bmatrix}
   * @f]
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   *
   * @return @b bool if the conversion was successful
   */
  bool RosettaOsirisCameraDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    // image coordinates prior to introducing distortion
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    // The equations are in pixel coordinates so convert
    double uxPixel = focalXToLine(ux);
    double uyPixel = focalYToSample(uy);

    // Loop setup
    double tolerance = 1e-7;
    int iteration = 1;
    int maxIterations = 20;
    bool done = false;

    LinearAlgebra::Vector undistortedCoordinate = LinearAlgebra::zeroVector(2);
    undistortedCoordinate(0) = uxPixel;
    undistortedCoordinate(1) = uyPixel;
    // Use the undistorted coordinate as the initial point
    LinearAlgebra::Vector distortedCoordinate = undistortedCoordinate;

    LinearAlgebra::Vector xTerms = LinearAlgebra::zeroVector(4);
    LinearAlgebra::Vector yTerms = LinearAlgebra::zeroVector(4);
    LinearAlgebra::Vector delXTerms = LinearAlgebra::zeroVector(4);
    LinearAlgebra::Vector delYTerms = LinearAlgebra::zeroVector(4);

    LinearAlgebra::Vector objectFuncValue = LinearAlgebra::zeroVector(2);
    LinearAlgebra::Vector updateStep = LinearAlgebra::zeroVector(2);
    LinearAlgebra::Matrix negJacobian = LinearAlgebra::zeroMatrix(2, 2);
    LinearAlgebra::Matrix negInvJacobian = LinearAlgebra::zeroMatrix(2, 2);

    // Compute the term vectors
    //   These are recomputed at the END of each iteration, so compute them
    //   before the first iteration
    xTerms(0) = 1.0;
    xTerms(1) = distortedCoordinate(0);
    xTerms(2) = distortedCoordinate(0)*distortedCoordinate(0);
    xTerms(3) = distortedCoordinate(0)*distortedCoordinate(0)*distortedCoordinate(0);

    yTerms(0) = 1.0;
    yTerms(1) = distortedCoordinate(1);
    yTerms(2) = distortedCoordinate(1)*distortedCoordinate(1);
    yTerms(3) = distortedCoordinate(1)*distortedCoordinate(1)*distortedCoordinate(1);

    delXTerms(1) = 1.0;
    delXTerms(2) = 2.0*distortedCoordinate(0);
    delXTerms(3) = 3.0*distortedCoordinate(0)*distortedCoordinate(0);

    delYTerms(1) = 1.0;
    delYTerms(2) = 2.0*distortedCoordinate(1);
    delYTerms(3) = 3.0*distortedCoordinate(1)*distortedCoordinate(1);

    // Compute the object function, the distance between the redistorted and undistorted
    objectFuncValue(0) = undistortedCoordinate(0)
                          - LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedX, xTerms),
                                                      yTerms );
    objectFuncValue(1) = undistortedCoordinate(1)
                          - LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedY, xTerms),
                                                      yTerms );

    while(!done) {

      // Compute the negative jacobian
      //   The variable terms and object function value have already been
      //   computed priort to checking for convergence at the end of the
      //   previous iteration. These are not needed to check for convergence
      //   so, compute them now.
      negJacobian(0, 0) = LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedX, delXTerms),
                                                     yTerms );
      negJacobian(0, 1) = LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedX, xTerms),
                                                     delYTerms );
      negJacobian(1, 0) = LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedY, delXTerms),
                                                     yTerms );
      negJacobian(1, 1) = LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedY, xTerms),
                                                     delYTerms );

      // Invert the negative jacobian
      //   If it is not invertible, then fail
      double det = negJacobian(0, 0) * negJacobian(1, 1) - negJacobian(1, 0) * negJacobian(0, 1);
      if ( fabs(det) < 1e-15 ) {
        return false;
      }
      negInvJacobian(0, 0) = negJacobian(1, 1) / det;
      negInvJacobian(0, 1) = - negJacobian(0, 1) / det;
      negInvJacobian(1, 0) = - negJacobian(1, 0) / det;
      negInvJacobian(1, 1) = negJacobian(0, 0) / det;

      // Compute the update step
      updateStep = LinearAlgebra::multiply(negInvJacobian, objectFuncValue);

      // Update and check for convergence
      distortedCoordinate += updateStep;

      // Compute the term vectors
      xTerms(0) = 1.0;
      xTerms(1) = distortedCoordinate(0);
      xTerms(2) = distortedCoordinate(0)*distortedCoordinate(0);
      xTerms(3) = distortedCoordinate(0)*distortedCoordinate(0)*distortedCoordinate(0);

      yTerms(0) = 1.0;
      yTerms(1) = distortedCoordinate(1);
      yTerms(2) = distortedCoordinate(1)*distortedCoordinate(1);
      yTerms(3) = distortedCoordinate(1)*distortedCoordinate(1)*distortedCoordinate(1);

      delXTerms(1) = 1.0;
      delXTerms(2) = 2.0*distortedCoordinate(0);
      delXTerms(3) = 3.0*distortedCoordinate(0)*distortedCoordinate(0);

      delYTerms(1) = 1.0;
      delYTerms(2) = 2.0*distortedCoordinate(1);
      delYTerms(3) = 3.0*distortedCoordinate(1)*distortedCoordinate(1);

      // Compute the object function, the distance between the redistorted and undistorted
      objectFuncValue(0) = undistortedCoordinate(0)
                           - LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedX, xTerms),
                                                        yTerms );
      objectFuncValue(1) = undistortedCoordinate(1)
                           - LinearAlgebra::dotProduct( LinearAlgebra::multiply(m_toUnDistortedY, xTerms),
                                                        yTerms );

      if ( LinearAlgebra::magnitude(objectFuncValue) < tolerance ||
           iteration > maxIterations ) {
        done = true;
      }
      iteration++;
    }

    p_focalPlaneX = lineToFocalX( distortedCoordinate(0) );
    p_focalPlaneY = sampleToFocalY( distortedCoordinate(1) );

    return true;
  }


  /**
   * Set the matrix for converting from distorted to undistorted samples.
   *
   * @param xMat The conversion matrix
   */
  void RosettaOsirisCameraDistortionMap::setUnDistortedXMatrix(LinearAlgebra::Matrix xMat) {
    m_toUnDistortedX = xMat;
  }


  /**
   * Set the matrix for converting from distorted to undistorted lines.
   *
   * @param yMat The conversion matrix
   */
  void RosettaOsirisCameraDistortionMap::setUnDistortedYMatrix(LinearAlgebra::Matrix yMat) {
    m_toUnDistortedY = yMat;
  }


  /**
   * Set the boresight location for converting from focal plane coordinates to
   * pixel coordinates.
   *
   * @param sample The sample location of the boresight
   * @param line The line location of the boresight
   */
  void RosettaOsirisCameraDistortionMap::setBoresight(double sample, double line) {
    m_boresightSample = sample;
    m_boresightLine = line;
  }


  /**
   * Set the pixel pitch for converting from focal plane coordinates to
   * pixel coordinates.
   *
   * @param pitch The pixel pitch in mm per pixel.
   */
  void RosettaOsirisCameraDistortionMap::setPixelPitch(double pitch) {
    m_pixelPitch = pitch;
  }


  /**
   * Convert a focal plane x coordinate to a pixel space line coordinate.
   *
   * @param x The focal plane x coordinate in mm
   *
   * @return @b double The pixel space line coordinate
   */
  double RosettaOsirisCameraDistortionMap::focalXToLine(double x) {
    return ( x / m_pixelPitch + m_boresightLine );
  }


  /**
   * Convert a focal plane y coordinate to a pixel space sample coordinate.
   *
   * @param y The focal plane y coordinate in mm
   *
   * @return @b double The pixel space sample coordinate
   */
  double RosettaOsirisCameraDistortionMap::focalYToSample(double y) {
    return ( y / m_pixelPitch + m_boresightSample);
  }


  /**
   * Convert pixel space line coordinate to a focal plane x coordinate.
   *
   * @param line The pixel space line coordinate
   *
   * @return @b double The focal plane x coordinate in mm
   */
  double RosettaOsirisCameraDistortionMap::lineToFocalX(double line) {
    return ( ( line - m_boresightLine ) * m_pixelPitch );
  }


  /**
   * Convert pixel space sample coordinate to a focal plane y coordinate.
   *
   * @param sample The pixel space sample coordinate
   *
   * @return @b double The focal plane y coordinate in mm
   */
  double RosettaOsirisCameraDistortionMap::sampleToFocalY(double sample) {
    return ( ( sample - m_boresightSample) * m_pixelPitch );
  }
}
