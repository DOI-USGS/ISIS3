#include "SurfacePoint.h"

#include <naif/SpiceUsr.h>

#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Constructs an empty SurfacePoint object
   *
   */
  SurfacePoint::SurfacePoint() {
    InitCovariance();
    InitPoint();
    InitRadii();
  }


  /**
   * Constructs a SurfacePoint object with a spherical point only
   *  
   * @param lat  The latitude of the surface point 
   * @param lon  The longitude of the surface point 
   * @param radius The radius of the surface point 
   */
  SurfacePoint::SurfacePoint(Latitude lat, Longitude lon, Distance radius) {
    InitRadii();
    InitCovariance();
    SetSpherical(lat, lon, radius);
  }



  /**
   * Constructs a SurfacePoint object with a spherical point and its sigmas
   *  
   * @param lat  The latitude of the surface point 
   * @param lon  The longitude of the surface point 
   * @param radius The radius of the surface point 
   *  
   *               The sigmas indicate the accuracy of the point.  For instance,
   *               a latitude sigma of 5 degrees would indicate that the
   *               latitiude value could have an error or + or - 5 degrees. 
   * @param sigmaLat  The sigma of the latitude 
   * @param sigmaLon  The sigma of the longitude 
   * @param sigmaRadius  The sigma of the local radius 
   */
  SurfacePoint::SurfacePoint(Latitude lat, Longitude lon, Distance radius,
                        Angle latSigma, Angle lonSigma, Distance radiusSigma) {
    InitRadii();
    InitCovariance();
    SetSpherical(lat, lon, radius, latSigma, lonSigma, radiusSigma);
  }


  /**
   * Constructs a SurfacePoint object with both a spherical point and 
   *   its variance/covariance matrix.
   *
   */
  SurfacePoint::SurfacePoint(Latitude lat, Longitude lon, Distance radius,
                             const symmetric_matrix<double,upper>& covar) {
    InitRadii();
    InitCovariance();
    SetSpherical(lat, lon, radius, covar);
  }


  /**
   * Constructs a SurfacePoint object with a rectangular point only
   *  
   * @param x  The x coordinate of the surface point 
   * @param y  The y coordinate of the surface point 
   * @param z  The z coordinate of the surface point 
   */
  SurfacePoint::SurfacePoint(Displacement x, Displacement y, Displacement z) {
    InitRadii();
    InitCovariance();
    SetRectangular(x, y, z);
  }


  /**
   * Constructs a SurfacePoint object with a rectangular point and sigmas
   *  
   * @param x  The x coordinate of the surface point 
   * @param y  The y coordinate of the surface point 
   * @param z  The z coordinate of the surface point 
   *  
   *           The sigmas indicate the accuracy of the point.  For instance,
   *           a sigmaX=100 m, would indicate that the x coordinate was accurate
   *           to within 100 meters.
   * @param xSigma  The x coordinate of the surface point 
   * @param ySigma  The y coordinate of the surface point 
   * @param zSigma  The z coordinate of the surface point 
   */
  SurfacePoint::SurfacePoint(Displacement x, Displacement y, Displacement z,
                            Distance xSigma, Distance ySigma, Distance zSigma) {
    InitRadii();
    InitCovariance();
    SetRectangular(x, y, z, xSigma, ySigma, zSigma);
  }


  /**
   * Constructs a SurfacePoint object with a rectangular point and its 
   *   variance/covariance matrix 
   *  
   * @param x  The x coordinate of the surface point 
   * @param y  The y coordinate of the surface point 
   * @param z  The z coordinate of the surface point 
   * @param covar  The variance/covariance matrix of the point 
   */
  SurfacePoint::SurfacePoint(Displacement x, Displacement y, Displacement z,
                             const symmetric_matrix<double,upper>& covar) {
    InitCovariance();
    InitRadii();
    SetRectangular(x, y, z, covar);
  }


  /**
   * Destroys a SurfacePoint object/ 
   *  
   */
  SurfacePoint::~SurfacePoint() {
    // Reset class members to obviously bad values to help debug memory problems
    p_x = Displacement();
    p_y = Displacement();
    p_z = Displacement();
    p_majorAxis = Distance();
    p_minorAxis = Distance();
    p_polarAxis = Distance();
  }


  /**
   * Initialize the variance/covariance matrices
   *
   */
  void SurfacePoint::InitCovariance() {
    p_rectCovar.resize(3);
    p_rectCovar.clear();
    p_sphereCovar.resize(3);
    p_sphereCovar.clear();
    p_hasMatrix = false;
  }


  /**
   * Initialize a surface point 
   *  
   */
  void SurfacePoint::InitPoint() {
    p_x = Displacement();
    p_y = Displacement();
    p_z = Displacement();
    p_hasPoint = false;
  }

  /**
   * Initialize the target surface radii 
   *  
   */
  void SurfacePoint::InitRadii() {
    p_majorAxis = Distance();
    p_minorAxis = Distance();
    p_polarAxis = Distance();
    p_hasRadii = false;
  }


  /**
   * This is a private method to set a surface point in rectangular, body-fixed 
   *   coordinates.  This method isolates the procedure for setting a
   *   rectangular point in one place.
   *  
   *  
   * @param x  x value of body-fixed coordinate of surface point  
   * @param y  y value of body-fixed coordinate of surface point
   * @param z  z value of body-fixed coordinate of surface point
   *  
   * @return void
   */
  void SurfacePoint::SetRectangularPoint(Displacement x, Displacement y,
                                         Displacement z) {

    if (!x.Valid() || !y.Valid() || !z.Valid()) {
      iString msg = "x, y, and z must be set to valid displacements.  One or "
        "more coordinates have been set to an invalid displacement.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    p_x = x;
    p_y = y;
    p_z = z;
    p_hasPoint = true;
  }


  /**
   * Set surface point in rectangular body-fixed coordinates wtih optional 
   *   sigmas. 
   *  
   *  
   * @param x  x value of body-fixed coordinate of surface point  
   * @param y  y value of body-fixed coordinate of surface point
   * @param z  z value of body-fixed coordinate of surface point
   * @param xSigma  x sigma of body-fixed coordinate of surface point  
   * @param ySigma  y sigma of body-fixed coordinate of surface point
   * @param zSigma  z sigma of body-fixed coordinate of surface point
   *  
   * @return void
   */
  void SurfacePoint::SetRectangular(Displacement x, Displacement y, Displacement z,
                          Distance xSigma, Distance ySigma, Distance zSigma) {
    SetRectangularPoint(x, y, z);

    if (xSigma != Distance()  &&  ySigma != Distance()  &&  zSigma != Distance()) 
      SetRectangularSigmas(xSigma, ySigma, zSigma); 
  }


  /**
   * Set surface point in rectangular coordinates with its variance/covariance 
   *   matrix in meters squared. 
   *  
   * @param x  x value of body-fixed coordinate of surface point  
   * @param y  y value of body-fixed coordinate of surface point
   * @param z  z value of body-fixed coordinate of surface point
   * @param covar Rectangular variance/covariance matrix in m*m
   *  
   * @return void
   */
  void SurfacePoint::SetRectangular(Displacement x, Displacement y, Displacement z, 
                                    const symmetric_matrix<double,upper>& covar) {
    SetRectangularPoint(x, y, z);
    SetRectangularMatrix(covar);
  }


  /**
   * Set the rectangular sigmas into the rectangular variance/covariance 
   *   matrix. 
   *  
   * @param xSigma x sigma of body-fixed coordinate of surface point 
   * @param ySigma y sigma of body-fixed coordinate of surface point 
   * @param zSigma z sigma of body-fixed coordinate of surface point 
   */
  void SurfacePoint::SetRectangularSigmas(Distance xSigma,
                                          Distance ySigma,
                                          Distance zSigma) {
    // Is this error checking necessary or should we just use Distance?????
    if (!xSigma.Valid() || !ySigma.Valid() || !zSigma.Valid()) {
      iString msg = "x sigma, y sigma , and z sigma must be set to valid "
        "distances.  One or more sigmas have been set to an invalid distance.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    symmetric_matrix<double,upper> covar(3);
    covar.clear();
    covar(0,0) = xSigma * xSigma;
    covar(1,1) = ySigma * ySigma;
    covar(2,2) = zSigma * zSigma;
    SetRectangularMatrix(covar);
  }


  /**
   * Set rectangular covariance matrix 
   *
   * @param covar Rectangular variance/covariance matrix (units are m**2)
   *
   * @return void
   */
  void SurfacePoint::SetRectangularMatrix(
       const symmetric_matrix<double,upper>& covar) {

    // Make sure the point is set first
    if (!p_hasPoint) {
      iString msg = "A point must be set before a variance/covariance matrix "
        "can be set.";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    p_rectCovar = covar;
    SpiceDouble rectMat[3][3];

    // Compute the local radius of the surface point
    double x2  =  p_x*p_x;
    double y2  =  p_y*p_y;
    double z = (double) p_z;
    double radius = sqrt(x2 + y2 + z*z);

    // Should we use a matrix utility?
    rectMat[0][0] = covar(0,0);
    rectMat[0][1] = rectMat[1][0] = covar(0,1);
    rectMat[0][2] = rectMat[2][0] = covar(0,2);
    rectMat[1][1] = covar(1,1);
    rectMat[1][2] = rectMat[2][1] = covar(1,2);
    rectMat[2][2] = covar(2,2);

    // Compute the Jacobian
    SpiceDouble J[3][3];
    double zOverR = p_z/radius;
    double r2 = radius*radius;
    double denom = r2*radius*sqrt(1.0 - (zOverR*zOverR));
    J[0][0] = -p_x*p_z / denom;
    J[0][1] = -p_y*p_z / denom;
    J[0][2] = (r2 - p_z*p_z)/denom;
    J[1][0] = -p_y/(x2 + y2);
    J[1][1] = p_x / (x2 + y2);
    J[1][2] = 0.0;
    J[2][0] = p_x / radius;
    J[2][1] = p_y/radius;
    J[2][2] = p_z/radius;

    SpiceDouble mat[3][3];
    mxm_c (J, rectMat, mat);
    mxmt_c (mat, J, mat);
    p_sphereCovar(0,0) = mat[0][0];
    p_sphereCovar(0,1) = mat[0][1];
    p_sphereCovar(0,2) = mat[0][2];
    p_sphereCovar(1,1) = mat[1][1];
    p_sphereCovar(1,2) = mat[1][2];
    p_sphereCovar(2,2) = mat[2][2];

    p_hasMatrix = true;
  }


  /**
   * This is a private method to set a surface point in spherical 
   *   (lat/lon/radius), body-fixed coordinates.  This method isolates the
   *   procedure for setting a spherical point in one place.
   *  
   * @param lat Body-fixed latitude of surface point 
   * @param lon Body-fixed longitude of surface point 
   * @param radius Local radius of surface point 
   *  
   * @return void
   */
  void SurfacePoint::SetSphericalPoint(Latitude lat,
                                       Longitude lon,
                                       Distance radius) {
// Is error checking necessary or does Latitude, Longitude, and Distance handle it?????
    if (!lat.Valid()  ||  !lon.Valid()  ||  !radius.Valid()) {
      iString msg = "Latitude, longitude, or radius is an invalid value.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    SpiceDouble dlat = (double) lat.GetRadians();
    SpiceDouble dlon = (double) lon.GetRadians();
    SpiceDouble dradius = radius.GetKilometers();

    SpiceDouble rect[3];

    latrec_c ( dradius, dlon, dlat, rect);
    SetRectangularPoint(Displacement(rect[0], Displacement::Kilometers),
                        Displacement(rect[1], Displacement::Kilometers),
                        Displacement(rect[2], Displacement::Kilometers));
  }


  /**
   * Set surface point in spherical body-fixed coordinates (lat/lon/radius) wtih 
   *   optional sigmas.
   *  
   * @param lat  Body-fixed latitude of surface point 
   * @param lon  Body-fixed longitude of surface point 
   * @param radius  Local radius of surface point 
   * @param latSigma  Latitude sigma of of spherical coordinate of surface point 
   * @param lonSigma  Longitude sigma of of spherical coordinate of surface 
   *                   point
   * @param radiusSigma  Local radius sigma of of spherical coordinate of 
   *                   surface point 
   */
  void SurfacePoint::SetSpherical(Latitude lat, Longitude lon, Distance radius,
                                  Angle latSigma,
                                  Angle lonSigma,
                                  Distance radiusSigma) {
    SetSphericalPoint(lat, lon, radius);

    if (latSigma != Angle()  &&  lonSigma != Angle()  &&
        radiusSigma != Distance()) 
      SetSphericalSigmas(latSigma, lonSigma, radiusSigma); 
  }


  /**
   * Set surface point in spherical body-fixed coordinates (lat/lon/radius) wtih 
   *   its variance/covariance matrix in radians squared.
   *  
   * @param lat  Body-fixed latitude of surface point 
   * @param lon  Body-fixed longitude of surface point 
   * @param radius  Local radius of surface point 
   * @param covar Spherical variance/covariance matrix in m*m
   */
  void SurfacePoint::SetSpherical(Latitude lat, Longitude lon, Distance radius,
                                  const symmetric_matrix<double,upper>& covar) {
    SetSphericalPoint(lat, lon, radius);
    SetSphericalMatrix(covar);
  }


  /**
   * Set the spherical sigmas into the spherical variance/covariance matrix.
   *  
   * @param latSigma Latitude sigma of body-fixed coordinate of surface point 
   * @param lonSigma Longitude sigma of body-fixed coordinate of surface point 
   * @param radiusSigma Radius sigma of body-fixed coordinate of surface point 
   */
  void SurfacePoint::SetSphericalSigmas(Angle latSigma,
                                        Angle lonSigma,
                                        Distance radiusSigma) {
    // Is any error checking necessary beyond Angle and Distance????

    symmetric_matrix<double,upper> covar(3);
    covar.clear();
    double sphericalCoordinate;
    sphericalCoordinate = (double) latSigma.GetRadians();
    covar(0,0) =  sphericalCoordinate*sphericalCoordinate;
    sphericalCoordinate = (double) lonSigma.GetRadians();
    covar(1,1) = sphericalCoordinate*sphericalCoordinate;
    sphericalCoordinate = (double) radiusSigma.GetMeters();
    covar(2,2) = sphericalCoordinate*sphericalCoordinate;

    SetSphericalMatrix(covar);
  }


  /**
   * Set the spherical sigmas (in meters) into the spherical variance/covariance 
   *   matrix.
   *  
   * @param latSigma Latitude sigma of body-fixed coordinate of surface point 
   *                  in meters 
   * @param lonSigma Longitude sigma of body-fixed coordinate of surface point 
   *                  in meters 
   * @param radiusSigma Radius sigma of body-fixed coordinate of surface point 
   *                  in meters 
   */
  void SurfacePoint::SetSphericalSigmasDistance(Distance latSigma,
                                        Distance lonSigma,
                                        Distance radiusSigma) {
    // Is any error checking necessary beyond Angle and Distance????

    if (!p_hasRadii) {
      iString msg = "In order to use sigmas in meter units, the equitorial "
        "radius must be set with a call to SetRadii or an appropriate "
        "constructor";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    double scaledLatSig = latSigma/p_majorAxis;
    double scaledLonSig = lonSigma*cos((double) GetLatitude().GetRadians())/p_majorAxis;
    SetSphericalSigmas( Angle(scaledLatSig), Angle(scaledLonSig), radiusSigma);
  }


  /**
   * Set spherical covariance matrix
   *
   * @param covar Spherical variance/covariance matrix (radians**2)
   *
   * @return void
   */
  void SurfacePoint::SetSphericalMatrix(
     const symmetric_matrix<double,upper>& covar) {

    // Make sure the point is set first
    if (!p_hasPoint) {
      iString msg = "A point must be set before a variance/covariance matrix "
        "can be set.";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    p_sphereCovar = covar;
    SpiceDouble sphereMat[3][3];

    sphereMat[0][0] = covar(0,0);
    sphereMat[0][1] = sphereMat[1][0] = covar(0,1);
    sphereMat[0][2] = sphereMat[2][0] = covar(0,2);
    sphereMat[1][1] = covar(1,1);
    sphereMat[1][2] = sphereMat[2][1] = covar(1,2);
    sphereMat[2][2] = covar(2,2);

//     std::cout<<"Ocovar = "<<sphereMat[0][0]<<" "<<sphereMat[0][1]<<" "<<sphereMat[0][2]<<std::endl
//              <<"         "<<sphereMat[1][0]<<" "<<sphereMat[1][1]<<" "<<sphereMat[1][2]<<std::endl
//              <<"         "<<sphereMat[2][0]<<" "<<sphereMat[2][1]<<" "<<sphereMat[2][2]<<std::endl;

    // Get the lat/lon/radius of the point
    double lat = (double) GetLatitude().GetRadians();
    double lon = (double) GetLongitude().GetRadians();
    double radius = (double) GetLocalRadius().GetMeters();

    // Compute the Jacobian
    SpiceDouble J[3][3];
    double cosPhi = cos(lat);
    double sinPhi = sin(lat);
    double cosLamda = cos(lon);
    double sinLamda = sin(lon);
    double rcosPhi = radius*cosPhi;
    double rsinPhi = radius*sinPhi;
    J[0][0] = -rsinPhi * cosLamda;
    J[0][1] = -rcosPhi * sinLamda;
    J[0][2] = cosPhi * cosLamda;
    J[1][0] = -rsinPhi * sinLamda;
    J[1][1] = rcosPhi * cosLamda;
    J[1][2] = cosPhi * sinLamda;
    J[2][0] = rcosPhi;
    J[2][1] = 0.0;
    J[2][2] = sinPhi;

    SpiceDouble mat[3][3];
    mxm_c (J, sphereMat, mat);
    mxmt_c (mat, J, mat);
    //  TODO  Test to see if only the upper triangular portion of the matrix needs to be set
    p_rectCovar(0,0) = mat[0][0];
    p_rectCovar(0,1) = mat[0][1];
    p_rectCovar(0,2) = mat[0][2];
    p_rectCovar(1,1) = mat[1][1];
    p_rectCovar(1,2) = mat[1][2];
    p_rectCovar(2,2) = mat[2][2];

//     std::cout<<"Rcovar = "<<p_rectCovar(0,0)<<" "<<p_rectCovar(0,1)<<" "<<p_rectCovar(0,2)<<std::endl
//              <<"         "<<p_rectCovar(1,0)<<" "<<p_rectCovar(1,1)<<" "<<p_rectCovar(1,2)<<std::endl
//              <<"         "<<p_rectCovar(2,0)<<" "<<p_rectCovar(2,1)<<" "<<p_rectCovar(2,2)<<std::endl;
  }


  /**
   * Reset the radii of the surface body of the surface point 
   *  
   * @param majorAxis  The semi-major axis of the surface model 
   * @param minorAxis  The semi-minor axis of the surface model 
   * @param polarAxis  The polar axis of the surface model 
   */
  void SurfacePoint::SetRadii(Distance majorRadius,
                         Distance minorRadius,
                         Distance polarRadius) {

    if (!majorRadius.Valid()  ||  
        !minorRadius.Valid()  ||  
        !polarRadius.Valid()) {
      iString msg = "Radii must be set to valid distances.  One or more radii "
        "have been set to an invalid distance.";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    p_majorAxis = majorRadius;
    p_minorAxis = minorRadius;
    p_polarAxis = polarRadius;
    p_hasRadii = true;
  }


  /**
   * This method resets the local radius of a SurfacePoint 
   *  
   * @param radius The new local radius value to set 
   *  
   */
  void SurfacePoint::ResetLocalRadius(Distance radius) {

    if (!radius.Valid()) {
      iString msg = "Radius value must be a valid Distance.";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if (!p_hasPoint) {
        iString msg = "In order to reset the local radius, a Surface Point must "
          "already be set.";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    SpiceDouble lat = (double) GetLatitude().GetRadians();
    SpiceDouble lon = (double) GetLongitude().GetRadians();
    SpiceDouble rect[3];
    latrec_c ((SpiceDouble) radius.GetKilometers(), lon, lat, rect);
    p_x.SetKilometers(rect[0]);
    p_y.SetKilometers(rect[1]);
    p_z.SetKilometers(rect[2]);

    // TODO What should be done to the variance/covariance matrix when the
    // radius is reset??? With Bundle updates will this functionality be
    // obsolete??? Ask Ken
  }


  /**
   * Return the body-fixed latitude for the surface point 
   *  
   */
    Latitude SurfacePoint::GetLatitude() const {
      if (!Valid()) 
        return Latitude();

      // TODO Scale for accuracy with coordinate of largest magnitude
      double x = (double) p_x;
      double y = (double) p_y;
      double z = (double) p_z;

      if (x != 0.  ||  y != 0.  || z != 0.) 
        return atan2(z, sqrt(x*x + y*y) );
      else
        return 0.;
    }


    /**
     * Return the body-fixed longitude for the surface point 
     *  
     */
    Longitude SurfacePoint::GetLongitude() const {
      if (!Valid()) 
        return Longitude();

      double x = (double) p_x;
      double y = (double) p_y;
      double z = (double) p_z;

      if (x != 0.  ||  y != 0.) {
        if (z != 0.){
          double lon = atan2(y, x);
          if (lon < 0) {
            lon += 2 * PI;
          }
          return lon;
        } else
          return 0.;
      }
      else
        return 0.;
    }


  /**
   * Return the radius of the surface point 
   *  
   */
    Distance SurfacePoint::GetLocalRadius() const {
      if (!Valid()) 
        return Distance();

      double x = (double) p_x;
      double y = (double) p_y;
      double z = (double) p_z;

      return sqrt(x*x + y*y + z*z);
    }


  /**
   * Return the latitude sigma in meters
   *  
   */
    Distance SurfacePoint::GetLatSigmaDistance() const {
      if (!p_hasRadii) {
        iString msg = "In order to calculate sigmas in meter units, the "
          "equitorial radius must be set with a call to SetRadii.";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      Angle latSigma = GetLatSigma();
      // Convert from radians to meters
      return Distance(latSigma*p_majorAxis);
    }


  /**
   * Return the longiitude sigma in meters
   *  
   */
  Distance SurfacePoint::GetLonSigmaDistance() const {
    if (!p_hasRadii) {
      iString msg = "In order to calculate sigmas in meter units, the "
          "equitorial radius must be set with a call to SetRadii.";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    Angle lonSigma = GetLonSigma();
    Latitude lat = GetLatitude();
    double scaler = cos(lat);

    // Convert from radians to meters and return
    if (scaler != 0.) 
      return Distance(lonSigma*p_majorAxis/scaler);
    else
      return Distance(0.);
  }


  bool SurfacePoint::operator==(const SurfacePoint &other) const {
    return p_hasPoint  == other.p_hasPoint  &&
           p_hasRadii  == other.p_hasRadii  &&
           p_hasMatrix == other.p_hasMatrix &&
           p_majorAxis == other.p_majorAxis &&
           p_minorAxis == other.p_minorAxis &&
           p_polarAxis == other.p_polarAxis &&
           p_x         == other.p_x         &&
           p_y         == other.p_y         &&
           p_z         == other.p_z         &&
           p_rectCovar(0, 0) == other.p_rectCovar(0, 0) &&
           p_rectCovar(0, 1) == other.p_rectCovar(0, 1) &&
           p_rectCovar(0, 2) == other.p_rectCovar(0, 2) &&
           p_rectCovar(1, 1) == other.p_rectCovar(1, 1) &&
           p_rectCovar(1, 2) == other.p_rectCovar(1, 2) &&
           p_rectCovar(2, 2) == other.p_rectCovar(2, 2) &&
           p_sphereCovar(0, 0) == other.p_sphereCovar(0, 0) &&
           p_sphereCovar(0, 1) == other.p_sphereCovar(0, 1) &&
           p_sphereCovar(0, 2) == other.p_sphereCovar(0, 2) &&
           p_sphereCovar(1, 1) == other.p_sphereCovar(1, 1) &&
           p_sphereCovar(1, 2) == other.p_sphereCovar(1, 2) &&
           p_sphereCovar(2, 2) == other.p_sphereCovar(2, 2);
  }
}
