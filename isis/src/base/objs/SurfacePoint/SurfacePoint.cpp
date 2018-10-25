#include "SurfacePoint.h"

#include <SpiceUsr.h>

#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"

using namespace boost::numeric::ublas;
using namespace std;

namespace Isis {

  /**
   * Constructs an empty SurfacePoint object
   *
   */
  SurfacePoint::SurfacePoint() {
    InitCovariance();
    InitPoint();
  }

  /**
   * Constructs a new SurfacePoint object from an existing SurfacePoint.
   *
   */
  SurfacePoint::SurfacePoint(const SurfacePoint &other) {
    p_localRadius = other.p_localRadius;

    if(other.p_x) {
      p_x = new Displacement(*other.p_x);
    }
    else {
      p_x = NULL;
    }

    if(other.p_y) {
      p_y = new Displacement(*other.p_y);
    }
    else {
      p_y = NULL;
    }

    if(other.p_z) {
      p_z = new Displacement(*other.p_z);
    }
    else {
      p_z = NULL;
    }

    if(other.p_rectCovar) {
      p_rectCovar = new symmetric_matrix<double, upper>(*other.p_rectCovar);
    }
    else {
      p_rectCovar = NULL;
    }

    if(other.p_sphereCovar) {
      p_sphereCovar = new symmetric_matrix<double, upper>(*other.p_sphereCovar);
    }
    else {
      p_sphereCovar = NULL;
    }
  }


  /**
   * Constructs a SurfacePoint object with a spherical point only
   *
   * @param lat  The latitude of the surface point
   * @param lon  The longitude of the surface point
   * @param radius The radius of the surface point
   */
  SurfacePoint::SurfacePoint(const Latitude &lat, const Longitude &lon,
      const Distance &radius) {
    InitCovariance();
    InitPoint();
    SetSphericalPoint(lat, lon, radius);
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
  SurfacePoint::SurfacePoint(const Latitude &lat, const Longitude &lon,
      const Distance &radius, const Angle &latSigma, const Angle &lonSigma,
      const Distance &radiusSigma) {
    InitCovariance();
    InitPoint();
    SetSpherical(lat, lon, radius, latSigma, lonSigma, radiusSigma);
  }


  /**
   * Constructs a SurfacePoint object with both a spherical point and
   *   its variance/covariance matrix.
   *
   */
  SurfacePoint::SurfacePoint(const Latitude &lat, const Longitude &lon,
      const Distance &radius, const symmetric_matrix<double, upper> &covar) {
    InitCovariance();
    InitPoint();
    SetSpherical(lat, lon, radius, covar);
  }


  /**
   * Constructs a SurfacePoint object with a rectangular point only
   *
   * @param x  The x coordinate of the surface point
   * @param y  The y coordinate of the surface point
   * @param z  The z coordinate of the surface point
   */
  SurfacePoint::SurfacePoint(const Displacement &x, const Displacement &y,
      const Displacement &z) {
    InitCovariance();
    InitPoint();
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
  SurfacePoint::SurfacePoint(const Displacement &x, const Displacement &y,
      const Displacement &z, const Distance &xSigma, const Distance &ySigma,
      const Distance &zSigma) {
    InitCovariance();
    InitPoint();
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
  SurfacePoint::SurfacePoint(const Displacement &x, const Displacement &y,
      const Displacement &z, const symmetric_matrix<double, upper> &covar) {
    InitCovariance();
    InitPoint();
    SetRectangular(x, y, z, covar);
  }


  /**
   * Destroys a SurfacePoint object/
   *
   */
  SurfacePoint::~SurfacePoint() {
    FreeAllocatedMemory();
  }


  /**
   * Initialize the variance/covariance matrices
   *
   */
  void SurfacePoint::InitCovariance() {
    p_rectCovar = NULL;
    p_sphereCovar = NULL;
  }


  /**
   * Initialize a surface point
   *
   */
  void SurfacePoint::InitPoint() {
    p_x = NULL;
    p_y = NULL;
    p_z = NULL;
    p_localRadius = Distance();
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
  void SurfacePoint::SetRectangularPoint(const Displacement &x,
      const Displacement &y, const Displacement &z) {
 
    if (!x.isValid() || !y.isValid() || !z.isValid()) {
      IString msg = "x, y, and z must be set to valid displacements.  One or "
        "more coordinates have been set to an invalid displacement.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if(p_x) {
      *p_x = x;
    }
    else {
      p_x = new Displacement(x);
    }

    if(p_y) {
      *p_y = y;
    }
    else {
      p_y = new Displacement(y);
    }

    if(p_z) {
      *p_z = z;
    }
    else {
      p_z = new Displacement(z);
    }

    // Added 07-30-2018 to avoid trying to set an invalid SurfacePoint.  This breaks the ringspt and cnetnewradii tests.  Also the pole...mean test.
    // if (!(this->Valid())) {
    //   IString msg = "All coodinates of the point are zero. At least one coordinate"
    //     " must be nonzero to be a valid SurfacePoint.";
    //   throw IException(IException::User, msg, _FILEINFO_);
    // }

    if (!p_localRadius.isValid()) {
      ComputeLocalRadius();
      p_localRadius = GetLocalRadius();
    }
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
  void SurfacePoint::SetRectangular(const Displacement &x,
      const Displacement &y, const Displacement &z, const Distance &xSigma,
      const Distance &ySigma, const Distance &zSigma) {
    
    // Wipe out current local radius to ensure it will be recalculated in SetRectangularPoint
     p_localRadius = Distance();
    
    SetRectangularPoint(x, y, z);

    if (xSigma.isValid() && ySigma.isValid() && zSigma.isValid())
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
    // Wipe out current local radius to ensure it will be recalulated in SetRectangularPoint
    p_localRadius = Distance();

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
  void SurfacePoint::SetRectangularSigmas(const Distance &xSigma,
                                          const Distance &ySigma,
                                          const Distance &zSigma) {
    if (!xSigma.isValid() || !ySigma.isValid() || !zSigma.isValid()) {
      IString msg = "x sigma, y sigma , and z sigma must be set to valid "
        "distances.  One or more sigmas have been set to an invalid distance.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    symmetric_matrix<double,upper> covar(3);
    covar.clear();
    covar(0,0) = xSigma.meters() * xSigma.meters();
    covar(1,1) = ySigma.meters() * ySigma.meters();
    covar(2,2) = zSigma.meters() * zSigma.meters();
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
       const symmetric_matrix<double, upper> &covar) {
    // Make sure the point is set first
    if (!Valid()) {
      IString msg = "A point must be set before a variance/covariance matrix "
        "can be set.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(p_rectCovar) {
      *p_rectCovar = covar;
    }
    else {
      p_rectCovar = new symmetric_matrix<double, upper>(covar);
    }

    SpiceDouble rectMat[3][3];

    // Compute the local radius of the surface point
    double x2  = p_x->meters() * p_x->meters();
    double y2  = p_y->meters() * p_y->meters();
    double z   = p_z->meters();
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
    double zOverR = p_z->meters() / radius;
    double r2 = radius*radius;
    double denom = r2*radius*sqrt(1.0 - (zOverR*zOverR));
    J[0][0] = -p_x->meters() * p_z->meters() / denom;
    J[0][1] = -p_y->meters() * p_z->meters() / denom;
    J[0][2] = (r2 - p_z->meters() * p_z->meters()) / denom;
    J[1][0] = -p_y->meters() / (x2 + y2);
    J[1][1] = p_x->meters() / (x2 + y2);
    J[1][2] = 0.0;
    J[2][0] = p_x->meters() / radius;
    J[2][1] = p_y->meters() / radius;
    J[2][2] = p_z->meters() / radius;

    if(!p_sphereCovar)
      p_sphereCovar = new symmetric_matrix<double, upper>(3);

    SpiceDouble mat[3][3];
    mxm_c (J, rectMat, mat);
    mxmt_c (mat, J, mat);
    (*p_sphereCovar)(0,0) = mat[0][0];
    (*p_sphereCovar)(0,1) = mat[0][1];
    (*p_sphereCovar)(0,2) = mat[0][2];
    (*p_sphereCovar)(1,1) = mat[1][1];
    (*p_sphereCovar)(1,2) = mat[1][2];
    (*p_sphereCovar)(2,2) = mat[2][2];
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
  void SurfacePoint::SetSphericalPoint(const Latitude  &lat,
                                       const Longitude &lon,
                                       const Distance  &radius) {
// Is error checking necessary or does Latitude, Longitude, and Distance handle it?????
    if (!lat.isValid()  ||  !lon.isValid()  ||  !radius.isValid()) {
      IString msg = "Latitude, longitude, or radius is an invalid value.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    SpiceDouble dlat = (double) lat.radians();
    SpiceDouble dlon = (double) lon.radians();
    SpiceDouble dradius = radius.kilometers();

    SpiceDouble rect[3];
    latrec_c ( dradius, dlon, dlat, rect);

    // Set local radius now since we have it to avoid calculating it later
    p_localRadius = radius;

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
  void SurfacePoint::SetSpherical(const Latitude &lat, const Longitude &lon,
      const Distance &radius, const Angle &latSigma, const Angle &lonSigma,
      const Distance &radiusSigma) {

    SetSphericalPoint(lat, lon, radius);

    if (latSigma.isValid() && lonSigma.isValid() && radiusSigma.isValid())
      SetSphericalSigmas(latSigma, lonSigma, radiusSigma);
  }


  /**
   * Set surface point in spherical body-fixed coordinates (lat/lon/radius) with
   *   its variance/covariance matrix in radians squared.
   *
   * @param lat  Body-fixed latitude of surface point
   * @param lon  Body-fixed longitude of surface point
   * @param radius  Local radius of surface point
   * @param covar Spherical variance/covariance matrix in m*m
   */
  void SurfacePoint::SetSpherical(const Latitude &lat, const Longitude &lon,
      const Distance &radius, const symmetric_matrix<double, upper> &covar) {
    SetSphericalPoint(lat, lon, radius);
    SetSphericalMatrix(covar);
  }


  /**
   * Update spherical coordinates (lat/lon/radius)
   *
   * @param lat
   * @param lon
   * @param radius
   *
   */
  void SurfacePoint::SetSphericalCoordinates(const Latitude &lat,
                                                const Longitude &lon, const Distance &radius) {
    SetSphericalPoint(lat, lon, radius);
  }


  /**
   * Set the spherical sigmas into the spherical variance/covariance matrix.
   *
   * @param latSigma Latitude sigma of body-fixed coordinate of surface point
   * @param lonSigma Longitude sigma of body-fixed coordinate of surface point
   * @param radiusSigma Radius sigma of body-fixed coordinate of surface point
   */
  void SurfacePoint::SetSphericalSigmas(const Angle &latSigma,
                                        const Angle &lonSigma,
                                        const Distance &radiusSigma) {
    if (latSigma.isValid() && lonSigma.isValid() && radiusSigma.isValid()) {
      symmetric_matrix<double,upper> covar(3);
      covar.clear();

      double sphericalCoordinate;
      sphericalCoordinate = (double) latSigma.radians();
      covar(0,0) =  sphericalCoordinate*sphericalCoordinate;
      sphericalCoordinate = (double) lonSigma.radians();
      covar(1,1) = sphericalCoordinate*sphericalCoordinate;
      sphericalCoordinate = (double) radiusSigma.meters();
      covar(2,2) = sphericalCoordinate*sphericalCoordinate;

      SetSphericalMatrix(covar);
    }
    else {
      delete p_sphereCovar;
      p_sphereCovar = NULL;

      delete p_rectCovar;
      p_rectCovar = NULL;
    }
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
   * @internal
   *   @history  2018-06-28 Debbie A. Cook  Revised to use the local radius of
   *                 the SurfacePoint to convert distance to angle instead of the
   *                 major equatorial axis.  Also corrected longitude conversion.
   *                 See note in SurfacePoint.h.  References #5457.
   */
  void SurfacePoint::SetSphericalSigmasDistance(const Distance &latSigma,
    const Distance &lonSigma, const Distance &radiusSigma) {

    if (!Valid()) {
      IString msg = "Cannot set spherical sigmas on an invalid surface point";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Convert Latitude sigma to radians
    double latSigRadians = latSigma / GetLocalRadius();

    // Convert Longitude sigma to radians
    double convFactor = cos((double)GetLatitude().radians());
    double lonSigRadians;
    
    if (convFactor > 0.0000000000000001) {             
      lonSigRadians = lonSigma / (convFactor*GetLocalRadius());
    }
    else {
      //  Brent Archinal suggested setting sigma to pi in the case of a point near the pole
      lonSigRadians = PI;
    }

    SetSphericalSigmas( Angle(latSigRadians, Angle::Radians),
                        Angle(lonSigRadians, Angle::Radians), radiusSigma);
  }


  /**
   * Set spherical covariance matrix
   *
   * @param covar Spherical variance/covariance matrix (radians**2 for lat and lon)
   *
   * @return void
   */
  void SurfacePoint::SetSphericalMatrix(
     const symmetric_matrix<double, upper> & covar) {

    // Make sure the point is set first
    if (!Valid()) {
      IString msg = "A point must be set before a variance/covariance matrix "
        "can be set.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(p_sphereCovar) {
      *p_sphereCovar = covar;
    }
    else {
      p_sphereCovar = new symmetric_matrix<double, upper>(covar);
    }

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
    double lat = (double) GetLatitude().radians();
    double lon = (double) GetLongitude().radians();
    double radius = (double) GetLocalRadius().meters();

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

    if(!p_rectCovar)
      p_rectCovar = new symmetric_matrix<double, upper>(3);

    SpiceDouble mat[3][3];
    mxm_c (J, sphereMat, mat);
    mxmt_c (mat, J, mat);
    //  TODO  Test to see if only the upper triangular portion of the matrix needs to be set
    (*p_rectCovar)(0,0) = mat[0][0];
    (*p_rectCovar)(0,1) = mat[0][1];
    (*p_rectCovar)(0,2) = mat[0][2];
    (*p_rectCovar)(1,1) = mat[1][1];
    (*p_rectCovar)(1,2) = mat[1][2];
    (*p_rectCovar)(2,2) = mat[2][2];

//     std::cout<<"Rcovar = "<<p_rectCovar(0,0)<<" "<<p_rectCovar(0,1)<<" "<<p_rectCovar(0,2)<<std::endl
//              <<"         "<<p_rectCovar(1,0)<<" "<<p_rectCovar(1,1)<<" "<<p_rectCovar(1,2)<<std::endl
//              <<"         "<<p_rectCovar(2,0)<<" "<<p_rectCovar(2,1)<<" "<<p_rectCovar(2,2)<<std::endl;
  }


  /**
   * A naif array is a c-style array of size 3. The element types are double...
   * keep in mind a SpiceDouble is a double. The values' units are
   * kilometers because that is the unit naif works in. The first element is X,
   * the second Y, and the third Z.
   *
   * @param naifOutput The naif array to populate with the surface point's
   *                   XYZ position.
   */
  void SurfacePoint::ToNaifArray(double naifOutput[3]) const {
    if(Valid()) {
      naifOutput[0] = p_x->kilometers();
      naifOutput[1] = p_y->kilometers();
      naifOutput[2] = p_z->kilometers();
    }
    else {
      IString msg = "Cannot convert an invalid surface point to a naif array";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * A naif array is a c-style array of size 3. The element types are double...
   * keep in mind a SpiceDouble is a double. The values' units are
   * kilometers because that is the unit naif works in. The first element is X,
   * the second Y, and the third Z. This loads the naif array into the surface
   * point.
   *
   * @param naifValues The naif array to use as rectangular coordinates
   */
  void SurfacePoint::FromNaifArray(const double naifValues[3]) {
    if(p_x && p_y && p_z) {
      p_x->setKilometers(naifValues[0]);
      p_y->setKilometers(naifValues[1]);
      p_z->setKilometers(naifValues[2]);
    }
    else {
      p_x = new Displacement(naifValues[0], Displacement::Kilometers);
      p_y = new Displacement(naifValues[1], Displacement::Kilometers);
      p_z = new Displacement(naifValues[2], Displacement::Kilometers);
    }
    
    ComputeLocalRadius();
    p_localRadius = GetLocalRadius();
  }


  /**
   * This method resets the local radius of a SurfacePoint
   *
   * @param radius The new local radius value to set
   *
   */
  void SurfacePoint::ResetLocalRadius(const Distance &radius) {

    if (!radius.isValid()) {
      IString msg = "Radius value must be a valid Distance.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (!p_x || !p_y || !p_z || !p_x->isValid() || !p_y->isValid() ||
        !p_z->isValid()) {
        IString msg = "In order to reset the local radius, a Surface Point must "
          "already be set.";
        throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Set latitudinal coordinates
    SpiceDouble lat = (double) GetLatitude().radians();
    SpiceDouble lon = (double) GetLongitude().radians();
    
    p_localRadius = radius;
    
    // Set rectangular coordinates
    SpiceDouble rect[3];
    latrec_c ((SpiceDouble) radius.kilometers(), lon, lat, rect);
    p_x->setKilometers(rect[0]);
    p_y->setKilometers(rect[1]);
    p_z->setKilometers(rect[2]);

    // TODO What should be done to the variance/covariance matrix when the
    // radius is reset??? With Bundle updates will this functionality be
    // obsolete??? Ask Ken
  }


  bool SurfacePoint::Valid() const {
    static const Displacement zero(0, Displacement::Meters);
    return p_x && p_y && p_z && p_x->isValid() && p_y->isValid() && p_z->isValid() &&
           (*p_x != zero || *p_y != zero || *p_z != zero);
  }


  Displacement SurfacePoint::GetX() const {
    if(!p_x) return Displacement();

    return *p_x;
  }


  Displacement SurfacePoint::GetY() const {
    if(!p_y) return Displacement();

    return *p_y;
  }


  Displacement SurfacePoint::GetZ() const {
    if(!p_z) return Displacement();

    return *p_z;
  }


  Distance SurfacePoint::GetXSigma() const {
    if(!p_rectCovar) return Distance();

    return Distance(sqrt((*p_rectCovar)(0, 0)), Distance::Meters);
  }


  Distance SurfacePoint::GetYSigma() const {
    if(!p_rectCovar) return Distance();

    return Distance(sqrt((*p_rectCovar)(1, 1)), Distance::Meters);
  }


  Distance SurfacePoint::GetZSigma() const {
    if(!p_rectCovar) return Distance();

    return Distance(sqrt((*p_rectCovar)(2, 2)), Distance::Meters);
  }


  symmetric_matrix<double, upper> SurfacePoint::GetRectangularMatrix()
      const {
    if(!p_rectCovar) {
      symmetric_matrix<double, upper> tmp(3);
      tmp.clear();
      return tmp;
    }

    return *p_rectCovar;
  }


  Angle SurfacePoint::GetLatSigma() const {
    if(!p_sphereCovar)
      return Angle();

    return Angle(sqrt((*p_sphereCovar)(0, 0)), Angle::Radians);
  }


  Angle SurfacePoint::GetLonSigma() const {
    if(!p_sphereCovar)
      return Angle();

    return Angle(sqrt((*p_sphereCovar)(1, 1)), Angle::Radians);
  }


  /**
   * Return the body-fixed latitude for the surface point
   *
   */
    Latitude SurfacePoint::GetLatitude() const {
      if (!Valid())
        return Latitude();

      // TODO Scale for accuracy with coordinate of largest magnitude
      double x = p_x->meters();
      double y = p_y->meters();
      double z = p_z->meters();

      if (x != 0.  ||  y != 0.  || z != 0.)
        return Latitude(atan2(z, sqrt(x*x + y*y) ), Angle::Radians);
      else
        return Latitude();
    }


    /**
     * Return the body-fixed longitude for the surface point
     *
     */
    Longitude SurfacePoint::GetLongitude() const {
      if (!Valid())
        return Longitude();

      double x = p_x->meters();
      double y = p_y->meters();

      if(x == 0.0 && y == 0.0) {
        return Longitude(0, Angle::Radians);
      }

      double lon = atan2(y, x);
      if (lon < 0) {
        lon += 2 * PI;
      }

      return Longitude(lon, Angle::Radians);
    }


  /**
   * Compute the local radius of the surface point
   *
   */
    void SurfacePoint::ComputeLocalRadius() {
    static const Displacement zero(0, Displacement::Meters);
      if (Valid()) {
        double x = p_x->meters();
        double y = p_y->meters();
        double z = p_z->meters();

        p_localRadius = Distance(sqrt(x*x + y*y + z*z), Distance::Meters);
      }
      else if (*p_x == zero && *p_y == zero && *p_z == zero) { // for backwards compatability
        p_localRadius = Distance(0., Distance::Meters);
      }
      else { // Invalid point
        IString msg = "SurfacePoint::Can't compute local radius on invalid point";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }


  /**
   * Return the radius of the surface point
   *
   */
    Distance SurfacePoint::GetLocalRadius() const {
     return p_localRadius;
    }


  /**
   * Return the latitude sigma in meters
   *
   */
  Distance SurfacePoint::GetLatSigmaDistance() const {
    Distance latSigmaDistance = Distance();

    if(Valid()) {
      Angle latSigma = GetLatSigma();

      if (latSigma.isValid() && GetLocalRadius().isValid()) {
        // Distance scalingRadius = GetLocalRadius();

        // Convert from radians to meters
        latSigmaDistance = latSigma.radians() * GetLocalRadius();
      }
    }

    return latSigmaDistance;
  }


  /**
   * Return the longitude sigma in meters
   *
   */
  Distance SurfacePoint::GetLonSigmaDistance() const {
    Distance lonSigmaDistance;

    if(Valid()) {
      Angle lonSigma = GetLonSigma();

      if (lonSigma.isValid()) {
        Distance scalingRadius = cos(GetLatitude().radians()) * GetLocalRadius();

        // Convert from radians to meters and return
        // TODO What do we do when the scaling radius is 0 (at the pole)?
        if (scalingRadius.meters() != 0.)
          lonSigmaDistance = lonSigma.radians() * scalingRadius;
      }
    }

    return lonSigmaDistance;
  }


  Distance SurfacePoint::GetLocalRadiusSigma() const {
    if(!p_sphereCovar)
      return Distance();

    return Distance(sqrt((*p_sphereCovar)(2, 2)), Distance::Meters);
  }


  symmetric_matrix<double, upper> SurfacePoint::GetSphericalMatrix() const {
    if(!p_sphereCovar) {
      symmetric_matrix<double, upper> tmp(3);
      tmp.clear();
      return tmp;
    }

    return *p_sphereCovar;
  }


  /**
   * Return latitude weight for bundle adjustment
   * Units are 1/(radians)^2
   *
   */
  double SurfacePoint::GetLatWeight() const {
    double dlatSigma = GetLatSigma().radians();

      if( dlatSigma <= 0.0 ) {
          IString msg = "SurfacePoint::GetLatWeight(): Sigma <= 0.0";
          throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      return 1.0/(dlatSigma*dlatSigma);
  }

  /**
  * Return longitude weight for bundle adjustment
  * Units are 1/(radians)^2
  *
  */
  double SurfacePoint::GetLonWeight() const {
    double dlonSigma = GetLonSigma().radians();

        if( dlonSigma <= 0.0 ) {
            IString msg = "SurfacePoint::GetLonWeight(): Sigma <= 0.0";
            throw IException(IException::Programmer, msg, _FILEINFO_);
        }

        return 1.0/(dlonSigma*dlonSigma);
      }

  /**
  * Return radius weight for bundle adjustment
  * Units are 1/(meters)^2
  *
  */
  double SurfacePoint::GetLocalRadiusWeight() const {

    double dlocalRadiusSigma = GetLocalRadiusSigma().kilometers();

        if (dlocalRadiusSigma <= 0.0 ) {
            IString msg = "SurfacePoint::GetRadWeight(): Sigma <= 0.0";
            throw IException(IException::Programmer, msg, _FILEINFO_);
        }

        return 1.0/(dlocalRadiusSigma*dlocalRadiusSigma);
      }

  /**
   * Computes and returns the distance between two surface points.  The average of 
   * the local radii will be used.
   */
  Distance SurfacePoint::GetDistanceToPoint(const SurfacePoint &other) const {
    
    if(!Valid() || !other.Valid())
      return Distance();

    return GetDistanceToPoint(other,
        ((GetLocalRadius() + other.GetLocalRadius()) / 2.0));
  }


  /**
   * Computes and returns the distance between two surface points,
   * assuming both points are on a sphere with the given radius.
   *
   * This uses the haversine formula to compute the distance.
   * Using a spherical model gives errors typically <1%
   */
  Distance SurfacePoint::GetDistanceToPoint(const SurfacePoint &other,
      const Distance &sphereRadius) const {
    if(!Valid() || !other.Valid())
      return Distance();

    // Convert lat/lon values to radians
    const Angle &latitude = GetLatitude();
    const Angle &longitude = GetLongitude();
    const Angle &otherLatitude = other.GetLatitude();
    const Angle &otherLongitude = other.GetLongitude();

    // The harvestine method:
    //   http://en.wikipedia.org/wiki/Haversine_formula
    Angle deltaLat = latitude - otherLatitude;
    Angle deltaLon = longitude - otherLongitude;

    double haversinLat = sin(deltaLat.radians() / 2.0);
    haversinLat *= haversinLat;

    double haversinLon = sin(deltaLon.radians() / 2.0);
    haversinLon *= haversinLon;

    double a = haversinLat + cos(latitude.radians()) *
               cos(otherLatitude.radians()) *
               haversinLon;

    double c = 2 * atan(sqrt(a) / sqrt(1 - a));

    return sphereRadius * c;
  }


  bool SurfacePoint::operator==(const SurfacePoint &other) const {
    bool equal = true;

    if(equal && p_x && p_y && p_z && other.p_x && other.p_y && other.p_z) {
      equal = equal && *p_x == *other.p_x;
      equal = equal && *p_y == *other.p_y;
      equal = equal && *p_z == *other.p_z;
    }
    else if(equal) {
      equal = equal && p_x == NULL && other.p_x == NULL;
      equal = equal && p_y == NULL && other.p_y == NULL;
      equal = equal && p_z == NULL && other.p_z == NULL;
    }

    if(equal && p_rectCovar) {
      equal = equal && (*p_rectCovar)(0, 0) == (*other.p_rectCovar)(0, 0);
      equal = equal && (*p_rectCovar)(0, 1) == (*other.p_rectCovar)(0, 1);
      equal = equal && (*p_rectCovar)(0, 2) == (*other.p_rectCovar)(0, 2);
      equal = equal && (*p_rectCovar)(1, 1) == (*other.p_rectCovar)(1, 1);
      equal = equal && (*p_rectCovar)(1, 2) == (*other.p_rectCovar)(1, 2);
      equal = equal && (*p_rectCovar)(2, 2) == (*other.p_rectCovar)(2, 2);
    }
    else if(equal) {
      equal = equal && p_rectCovar == NULL && other.p_rectCovar == NULL;
    }

    if(equal && p_sphereCovar) {
      equal = equal && (*p_sphereCovar)(0, 0) == (*other.p_sphereCovar)(0, 0);
      equal = equal && (*p_sphereCovar)(0, 1) == (*other.p_sphereCovar)(0, 1);
      equal = equal && (*p_sphereCovar)(0, 2) == (*other.p_sphereCovar)(0, 2);
      equal = equal && (*p_sphereCovar)(1, 1) == (*other.p_sphereCovar)(1, 1);
      equal = equal && (*p_sphereCovar)(1, 2) == (*other.p_sphereCovar)(1, 2);
      equal = equal && (*p_sphereCovar)(2, 2) == (*other.p_sphereCovar)(2, 2);
    }
    else if(equal) {
      equal = equal && p_sphereCovar == NULL && other.p_sphereCovar == NULL;
    }

    return equal;
  }

  SurfacePoint &SurfacePoint::operator=(const SurfacePoint &other) {
    // The lazy way of doing this (free all memory and copy) is too expensive
    // in the default case!
    if(p_x && other.p_x &&
       p_y && other.p_y &&
       p_z && other.p_z &&
       !p_rectCovar && !other.p_rectCovar &&
       !p_sphereCovar && !other.p_sphereCovar) {
      *p_x = *other.p_x;
      *p_y = *other.p_y;
      *p_z = *other.p_z;
    }
    else {
      FreeAllocatedMemory();

      if(other.p_x) {
        p_x = new Displacement(*other.p_x);
      }

      if(other.p_y) {
        p_y = new Displacement(*other.p_y);
      }

      if(other.p_z) {
        p_z = new Displacement(*other.p_z);
      }

      if(other.p_rectCovar) {
        p_rectCovar = new symmetric_matrix<double, upper>(*other.p_rectCovar);
      }

      if(other.p_sphereCovar) {
        p_sphereCovar = new symmetric_matrix<double, upper>(*other.p_sphereCovar);
      }
    }

    // Finally initialize local radius to avoid using a previous value
    p_localRadius = other.GetLocalRadius();

    return *this;
  }

  void SurfacePoint::FreeAllocatedMemory() {
    if(p_x) {
      delete p_x;
      p_x = NULL;
    }

    if(p_y) {
      delete p_y;
      p_y = NULL;
    }

    if(p_z) {
      delete p_z;
      p_z = NULL;
    }

    if(p_rectCovar) {
      delete p_rectCovar;
      p_rectCovar = NULL;
    }

    if(p_sphereCovar) {
      delete p_sphereCovar;
      p_sphereCovar = NULL;
    }
  }
}
