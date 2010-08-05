/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/03/27 06:36:41 $
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
#include "CameraGroundMap.h"
#include "NaifStatus.h"

namespace Isis {
  CameraGroundMap::CameraGroundMap(Camera *parent) {
    p_camera = parent;
    p_camera->SetGroundMap(this);
  }

  /** Compute ground position from focal plane coordinate
   *
   * This method will compute the ground position given an
   * undistorted focal plane coordinate.  Note that the latitude/longitude
   * value can be obtained from the camera class passed into the constructor.
   *
   * @param ux distorted focal plane x in millimeters
   * @param uy distorted focal plane y in millimeters
   * @param uz distorted focal plane z in millimeters
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::SetFocalPlane(const double ux, const double uy,
                                      double uz) {
    NaifStatus::CheckErrors();

    SpiceDouble lookC[3];
    lookC[0] = ux;
    lookC[1] = uy;
    lookC[2] = uz;

    SpiceDouble unitLookC[3];
    vhat_c(lookC, unitLookC);

    NaifStatus::CheckErrors();

    return p_camera->SetLookDirection(unitLookC);
  }

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::SetGround(const double lat, const double lon) {
    if(p_camera->Sensor::SetUniversalGround(lat, lon)) {
      LookCtoFocalPlaneXY();
      return true;
    }
    return false;
  }

  //! Compute undistorted focal plane coordinate from camera look vector
  void CameraGroundMap::LookCtoFocalPlaneXY() {
    double lookC[3];
    p_camera->Sensor::LookDirection(lookC);
    double scale = p_camera->FocalLength() / lookC[2];
    p_focalPlaneX = lookC[0] * scale;
    p_focalPlaneY = lookC[1] * scale;
  }

  /** Compute undistorted focal plane coordinate from ground position that includes a local radius
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in meters
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::SetGround(const double lat, const double lon, const double radius) {
    if(p_camera->Sensor::SetUniversalGround(lat, lon, radius)) {
      LookCtoFocalPlaneXY();
      return true;
    }
    return false;
  }


  /** Compute undistorted focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the undistorted focal plane coordinate for
   * a ground position, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/x/y.  The
   * class value for p_look is set by this method.
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in m
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::GetXY(const double lat, const double lon, const double radius,
                              double *cudx, double *cudy) {

    // Compute the look vector in body-fixed coordinates
    double pB[3]; // Point on surface
    latrec_c(radius / 1000.0, lon * Isis::PI / 180.0, lat * Isis::PI / 180.0, pB);
    return GetXY (pB, cudx, cudy);
  }

  /** Compute undistorted focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the undistorted focal plane coordinate for
   * a ground position, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/p_pB/x/y.  The
   * class value for p_look is set by this method.
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in m
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::GetXY(const double pB[3], double *cudx, double *cudy) {

    // Check for Sky images
    if(p_camera->IsSky()) {
      return false;
    }

    // Should a check be added to make sure SetImage has been called???

    // Compute the look vector in body-fixed coordinates
//    double pB[3]; // Point on surface
//    latrec_c(radius / 1000.0, lon * Isis::PI / 180.0, lat * Isis::PI / 180.0, pB);

    // Get spacecraft vector in body-fixed coordinates
    SpiceRotation *bodyRot = p_camera->BodyRotation();
    SpiceRotation *instRot = p_camera->InstrumentRotation();
    std::vector<double> sB = bodyRot->ReferenceVector(p_camera->InstrumentPosition()->Coordinate());
    std::vector<double> lookB(3);
    for(int ic = 0; ic < 3; ic++)   lookB[ic] = pB[ic] - sB[ic];

    // Check for point on back of planet by checking to see if surface point is viewable (test emission angle)
    // During iterations, we may not want to do the back of planet test???
    double upsB[3], upB[3], dist;
    vminus_c((SpiceDouble *) &lookB[0], upsB);
    unorm_c(upsB, upsB, &dist);
    unorm_c(pB, upB, &dist);
    double angle = vdot_c(upB, upsB);
    double emission;
    if(angle > 1) {
      emission = 0;
    }
    else if(angle < -1) {
      emission = 180.;
    }
    else {
      emission = acos(angle) * 180.0 / Isis::PI;
    }
    if(fabs(emission) > 90.) return false;

    // Get the look vector in the camera frame and the instrument rotation
    p_lookJ.resize(3);
    p_lookJ = p_camera->BodyRotation()->J2000Vector(lookB);
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    // Get focal length with direction for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    *cudx = lookC[0] * fl / lookC[2];
    *cudy = lookC[1] * fl / lookC[2];

    return true;
  }

  /** Compute derivative w/r to position of focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to a spacecraft position coordinate, using the current
   * Spice settings (time and kernels) without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  //  also have a GetDxyDorientation and a GetDxyDpoint
  bool CameraGroundMap::GetdXYdPosition(const SpicePosition::PartialType varType, int coefIndex,
                                        double *dx, double *dy) {

    //  TODO  add a check to make sure p_lookJ has been set

    // Get directional fl for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    // Rotate look vector into camera frame
    SpiceRotation *instRot = p_camera->InstrumentRotation();
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    SpicePosition *instPos = p_camera->InstrumentPosition();

    std::vector<double> d_lookJ = instPos->CoordinatePartial(varType, coefIndex);
    for(int j = 0; j < 3; j++) d_lookJ[j] *= -1.0;
    std::vector<double> d_lookC =  instRot->ReferenceVector(d_lookJ);
    *dx = fl * DQuotient(lookC, d_lookC, 0);
    *dy = fl * DQuotient(lookC, d_lookC, 1);
    return true;
  }

  /** Compute derivative of focal plane coordinate w/r to orientation from ground position using current Spice from SetImage call
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to the instrument orientation, using the current Spice
   * settings (time and kernels) without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  //  also have a GetDxyDorientation and a GetDxyDpoint
  bool CameraGroundMap::GetdXYdOrientation(const SpiceRotation::PartialType varType, int coefIndex,
      double *dx, double *dy) {

    //  TODO  add a check to make sure p_lookJ has been set

    // Get directional fl for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    // Rotate look vector into camera frame
    SpiceRotation *instRot = p_camera->InstrumentRotation();
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    std::vector<double> d_lookC = instRot->ToReferencePartial(p_lookJ, varType, coefIndex);
    *dx = fl * DQuotient(lookC, d_lookC, 0);
    *dy = fl * DQuotient(lookC, d_lookC, 1);
    return true;
  }

  /** Compute derivative of focal plane coordinate w/r to ground point from ground position using current Spice from SetImage call
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to lat, lon, or radius, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::GetdXYdPoint(double lat, double lon, double radius, PartialType wrt,
                                     double *dx, double *dy) {

    //  TODO  add a check to make sure p_lookJ has been set

    // Get directional fl for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    // Rotate look vector into camera frame
    SpiceRotation *instRot = p_camera->InstrumentRotation();
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    // Get the partial derivative of the surface point
    std::vector<double> d_lookB = PointPartial(lat, lon, radius, wrt);

    SpiceRotation *bodyRot = p_camera->BodyRotation();
    std::vector<double> d_lookJ = bodyRot->J2000Vector(d_lookB);
    std::vector<double> d_lookC = instRot->ReferenceVector(d_lookJ);

    *dx = fl * DQuotient(lookC, d_lookC, 0);
    *dy = fl * DQuotient(lookC, d_lookC, 1);
    return true;
  }


  /** Compute derivative with respect to indicated variable of conversion function from lat/lon/rad to rectangular coord
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in meters
   * @param wrt take derivative with respect to this value
   *
   * @return partialDerivative
   */
  std::vector<double> CameraGroundMap::PointPartial(double lat, double lon, double radius, PartialType wrt) {
    double rlat = lat * Isis::PI / 180.0;
    double rlon = lon * Isis::PI / 180.0;
    double sinLon = sin(rlon);
    double cosLon = cos(rlon);
    double sinLat = sin(rlat);
    double cosLat = cos(rlat);
    double radkm = radius / 1000.0;

    std::vector<double> v(3);
    if(wrt == WRT_Latitude) {
      v[0] = -radkm * sinLat * cosLon;
      v[1] = -radkm * sinLon * sinLat;
      v[2] =  radkm * cosLat;
    }
    else if(wrt == WRT_Longitude) {
      v[0] = -radkm * cosLat * sinLon;
      v[1] =  radkm * cosLat * cosLon;
      v[2] =  0.0;
    }
    else {
      v[0] = cosLon * cosLat;
      v[1] = sinLon * cosLat;
      v[2] = sinLat;
    }

    return v;
  }


  /**
   * Convenience method for quotient rule applied to look vector
   *
   * This method will compute the derivative of the following function
   * (coordinate x or y) / (coordinate z)
   *
   * @param look  look vector in camera frame
   * @param dlook derivative of look vector in camera frame
   * @param index vector value to differentiate
   *
   * @return derivative
   */
  double CameraGroundMap::DQuotient(std::vector<double> &look,
                                    std::vector<double> &dlook,
                                    int index) {
    return (look[2] * dlook[index] - look[index] * dlook[2]) /
           (look[2] * look[2]);
  }
}
