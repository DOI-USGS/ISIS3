/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/05/22 00:08:59 $
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

#include "Sensor.h"

#include <QDebug>

#include <iomanip>

#include "Angle.h"
#include "Constants.h"
#include "CubeManager.h"
#include "Distance.h"
#include "EllipsoidShape.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Projection.h"
#include "ShapeModel.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "Target.h"
#include "UniqueIOCachingAlgorithm.h"

#define MAX(x,y) (((x) > (y)) ? (x) : (y))

using namespace std;

namespace Isis {

  /**
   * Constructs a Sensor object and loads SPICE kernels using information from
   * the label object. The constructor expects an Instrument and Kernels group
   * to be in the labels (see Spice documentation).
   *
   * @param lab Label containing Instrument and Kernels groups.
   */
  Sensor::Sensor(Cube &cube) : Spice(cube) {
  }

  //! Destroys the Sensor
  Sensor::~Sensor() {
  }


  /**
   * This allows you to ignore the cube elevation model and use the ellipse
   *
   * @param ignore True if the elevation model is ignored
   */
  void Sensor::IgnoreElevationModel(bool ignore) {
    // if we have an elevation model and are not ignoring it,
    //   set p_hasElevationModel to true
    if (!ignore) {
      target()->restoreShape();
    }
    else {
      target()->setShapeEllipsoid();
    }
  }


  /**
  * By setting the time you essential set the position of the
  * spacecraft and body as indicated in the class Spice. However, after this
  * is invoked there will be no intersection point until SetLookDirection or
  * SetUniversalGround is invoked.
  *
  * @param time Ephemeris time (read NAIF documentation for a detailed
  *             description)
  */
  void Sensor::setTime(const iTime &time) {
    Spice::setTime(time);
    target()->shape()->clearSurfacePoint();
  }

  /**
   * Sets the look direction of the spacecraft. This routine will then attempt to
   * intersect the look direction with the target. If successful you can utilize
   * the methods which return the lat/lon, phase, incidence, etc. This routine
   * returns false if the look direction does not intersect the target.
   *
   * @param v[] A look vector in camera coordinates. For example, (0,0,1) is
   *            usually the look direction out of the boresight of a camera.
   *
   * @return @b bool True if the look direction intersected the target.
   * @internal
   *   @history  2009-09-23  Tracie Sucharski - Convert negative longitudes
   *                           returned my reclat.
   *   @history  2010-09-15  Janet Barrett - Modified this method to use a new
   *                           algorithm for finding the intersection of a ray with
   *                           the DEM. This was required to take care of problems that
   *                           were encountered at the poles of global DEM files. The
   *                           algorithm that is being used was taken from "Intersection
   *                           between spacecraft viewing vectors and digital elevation
   *                           models" by N.A. Teanby. This algorithm only works on
   *                           Equatorial Cylindrical projections. Other projections still
   *                           use the previous algorithm.
   *   @history  2010-10-26  Janet Barrett - The tolerance value of 1E-5 was too
   *                           small and was causing a divide by zero error when
   *                           the current and last intersections were virtually
   *                           the same. The tolerance was changed to 3E-8 * a
   *                           (where a is the equatorial radius of the planet
   *                           we are dealing with).
   *   @history 2011-01-24   Janet Barrett - Got rid of extra loop that wasn't
   *                         needed for the new ray tracing algorithm.
   *
   *   @history 2011-08-16  Jeff Anderson - Fixed a problem with an infinite
   *                           loop in the ray tracing algorithm.  The problem
   *                           was first exposed when trying to intersect the
   *                           Vesta DEM on the limb.
   * @return bool
   */
  bool Sensor::SetLookDirection(const double v[3]) {
    // The look vector must be in the camera coordinate system

    // copy v to LookC
    // lookC[0] = v[0];
    // lookC[1] = v[1];
    // lookC[2] = v[2];
    vector<double> lookC(v, v + 3);

    // Convert it to body-fixed
    const vector<double> &lookJ = instrumentRotation()->J2000Vector(lookC);
    const vector<double> &lookB = bodyRotation()->ReferenceVector(lookJ);

    // This memcpy does:
    // m_lookB[0] = lookB[0];
    // m_lookB[1] = lookB[1];
    // m_lookB[2] = lookB[2];
    memcpy(m_lookB, &lookB[0], sizeof(double) * 3);
    m_newLookB = true;

    // Don't try to intersect the sky
    if (target()->isSky()) {
      target()->shape()->setHasIntersection(false);
      return false;
    }

    // See if it intersects the planet
    const vector<double> &sB = bodyRotation()->ReferenceVector(
        instrumentPosition()->Coordinate());

    // double tolerance = resolution() / 100.0; return
    // target()->shape()->intersectSurface(sB, lookB, tolerance);
    return target()->shape()->intersectSurface(sB, lookB);
  }

  /**
   * Returns if the last call to either SetLookDirection or
   * SetUniversalGround had a valid intersection with the target. If so then
   * other methods such as Coordinate, UniversalLatitude, UniversalLongitude,
   * etc can be used with confidence.
   *
   * @return @b bool True if the look direction intersects with the
   *         target.
   */
  bool Sensor::HasSurfaceIntersection() const {
    return target()->shape()->hasIntersection();
  }


  /**
   * Returns the x,y,z of the surface intersection in BodyFixed km.
   *
   * @param p[] The coordinate of the surface intersection
   */
  void Sensor::Coordinate(double p[3]) const {
    ShapeModel *shape = target()->shape();
    p[0] = shape->surfaceIntersection()->GetX().kilometers();
    p[1] = shape->surfaceIntersection()->GetY().kilometers();
    p[2] = shape->surfaceIntersection()->GetZ().kilometers();
  }

  /**
   * Returns the planetocentric latitude at the surface intersection point
   * in body fixed.
   *
   * @return @b double Universal latitude value
   */
  double Sensor::UniversalLatitude() const {
    return target()->shape()->surfaceIntersection()->GetLatitude().degrees();
  }


  /**
   * Returns the latitude.
   */
  Latitude Sensor::GetLatitude() const {
    return target()->shape()->surfaceIntersection()->GetLatitude();
  }


  /**
   * Returns a positive east, 0-360 domain longitude at the surface
   * intersection point in body fixed.
   *
   * @return @b double Universal longitude value
   */
  double Sensor::UniversalLongitude() const {
    return target()->shape()->surfaceIntersection()->GetLongitude().degrees();
  }


  /**
   * Returns the longitude.
   */
  Longitude Sensor::GetLongitude() const {
    return target()->shape()->surfaceIntersection()->GetLongitude();
  }

  /**
   * Returns the surface point (most efficient accessor).
   */
  SurfacePoint Sensor::GetSurfacePoint() const {
    return *(target()->shape()->surfaceIntersection());
  }


  /**
   * Returns the local radius at the intersection point. This is either the
   * radius on the ellipsoid, the radius from the surface model passed into
   * the constructor, or the radius set with SetUniversalGround.
   */
  Distance Sensor::LocalRadius() const {
    //TODO: We probably need to be validating surface intersect point is not NULL
    // Here? or in ShapeModel? or what, man?
    return target()->shape()->surfaceIntersection()->GetLocalRadius();
  }


  /**
   * Returns the local radius at the intersection point. This is either the
   * radius on the ellipsoid, the radius from the surface model passed into
   * the constructor, or the radius set with SetUniversalGround.
   *
   * @return @b Distance The distance from the center of the planet to this
   *          lat,lon in meters
   */
  Distance Sensor::LocalRadius(Latitude lat, Longitude lon) {
    return target()->shape()->localRadius(lat, lon); 
  }


   /**
    * Returns the local radius at the intersection point. This is either the
    * radius on the ellipsoid, the radius from the surface model passed into
    * the constructor, or the radius set with SetUniversalGround.
    *
    * @return @b Distance The distance from the center of the planet to this
    *          lat,lon in meters
    */
  Distance Sensor::LocalRadius(double lat, double lon) {
    return target()->shape()->localRadius(Latitude(lat, Angle::Degrees),
                                Longitude(lon, Angle::Degrees));
  }

  /**
   * Returns the phase angle in degrees. This does not use the surface model.
   *
   * @return @b double Phase angle, in degrees.
   */
  double Sensor::PhaseAngle() const {
    std::vector<double> sunB(m_uB, m_uB+3);
    return target()->shape()->phaseAngle(
                               bodyRotation()->ReferenceVector(instrumentPosition()->Coordinate()), sunB);
  }


  /**
   * Returns the emission angle in degrees.
   *
   * @return @b double Emission angle, in degrees.
   */
  double Sensor::EmissionAngle() const {
    return target()->shape()->emissionAngle(
        bodyRotation()->ReferenceVector(instrumentPosition()->Coordinate()));
  }


  /**
   * Returns the incidence angle in degrees. This does not use the surface model.
   *
   * @return @b double Incidence angle, in degrees.
   */
  double Sensor::IncidenceAngle() const {
    std::vector<double> sunB(m_uB, m_uB+3);
    return target()->shape()->incidenceAngle(sunB);
  }

  /**
   * This is the opposite routine for SetLookDirection. Instead of computing a
   * point on the target, a point is set and the look direction is computed.
   * Other methods such as lat/lon, phase, incidence, etc. can be used if this
   * method returns a true.
   *
   * @param latitude Planetocentric latitude
   * @param longitude Positive east longitude
   * @param backCheck If true this method will check the lat/lon point to see if
   *                  it falls on the backside of the target (or beyond the
   *                  horizon). If false this test will not occur.
   *                  Defaults to true
   *
   * @return bool True if the look direction intersects the target.
   */
  bool Sensor::SetUniversalGround(const double latitude,
                                  const double longitude, bool backCheck) {

    ShapeModel *shape = target()->shape();
    shape->clearSurfacePoint();

    // Can't intersect the sky
    if (target()->isSky()) {
      return false;
    }

    // Load the latitude/longitude
    Latitude lat(latitude, Angle::Degrees);
    Longitude lon(longitude, Angle::Degrees);
    shape->surfaceIntersection()->SetSpherical(lat, lon, LocalRadius(lat, lon));
    return SetGroundLocal(backCheck);
  }

  /**
   * This overloaded method has the opposite function as SetLookDirection. Instead
   * of computing a point on the target, a point is set and the look direction is
   * computed.  Other methods such as lat/lon, phase, incidence, etc. can be used if
   * this method returns a true.
   *
   * @param latitude Planetocentric latitude in degrees
   * @param longitude Positive east longitude in degrees
   * @param radius Radius in meters
   * @param backCheck If true this method will check the lat/lon point to see if
   *                  it falls on the backside of the target (or beyond the
   *                  horizon). If false this test will not occur.
   *                  Defaults to true
   *
   * @return bool True if the look direction intersects the target.
   */
  bool Sensor::SetUniversalGround(const double latitude,
                                  const double longitude,
                                  const double radius, bool backCheck) {

    ShapeModel *shape = target()->shape();
    shape->clearSurfacePoint();

   // Can't intersect the sky
    if (target()->isSky()) {
      return false;
    }

    Latitude lat(latitude, Angle::Degrees);
    Longitude lon(longitude, Angle::Degrees);
    Distance rad(radius, Distance::Meters);
    shape->surfaceIntersection()->SetSpherical(lat, lon, rad);

    return SetGroundLocal(backCheck);
  }


  /**
   * This overloaded method has the opposite function as SetLookDirection. Instead
   * of computing a point on the target, a point is set and the look direction is
   * computed.  Other methods such as lat/lon, phase, incidence, etc. can be used if
   * this method returns a true.
   *
   * @param backCheck If true this method will check the lat/lon point to see if
   *                  it falls on the backside of the target (or beyond the
   *                  horizon). If false this test will not occur.
   *                  Defaults to true
   *
   * @return bool
   */
  bool Sensor::SetGround(const SurfacePoint &surfacePt, bool backCheck) {
    ShapeModel *shape = target()->shape();
    shape->clearSurfacePoint();

    // Can't intersect the sky
    if (target()->isSky()) {
      return false;
    }

    shape->setSurfacePoint(surfacePt);

    return SetGroundLocal(backCheck);
  }

  /**
  * This method handles the common functions for the overloaded SetUniversalGround methods.
  * Instead of computing a point on the target, a point is set (lat,lon,radius) and the look
  * direction is computed.
  *
  * @param backCheck If true this method will check the lat/lon point to see if
  *                  it falls on the backside of the target (or beyond the
  *                  horizon). If false this test will not occur.
  *                  Defaults to true
  *
  * @return bool True if the look direction intersects the target.
  */
  bool Sensor::SetGroundLocal(bool backCheck) {
    ShapeModel *shape = target()->shape();
    // With the 3 spherical value compute the x/y/z coordinate
    //latrec_c(m_radius, (m_longitude * PI / 180.0), (m_latitude * PI / 180.0), m_pB);


    if (!(shape->surfaceIntersection()->Valid())) {
      return false;
    }

    // Make sure the point isn't on the backside of the body

    // This is static purely for performance reasons. A significant speedup
    // is achieved here.
    const vector<double> &sB =
        bodyRotation()->ReferenceVector(instrumentPosition()->Coordinate());

    m_lookB[0] = shape->surfaceIntersection()->GetX().kilometers() - sB[0];
    m_lookB[1] = shape->surfaceIntersection()->GetY().kilometers() - sB[1];
    m_lookB[2] = shape->surfaceIntersection()->GetZ().kilometers() - sB[2];
    m_newLookB = true;

    // See if the point is on the backside of the target

    if (backCheck) {
      // Assume the intersection point is good in order to get the emission angle
      shape->setHasIntersection(true);
      if (fabs(shape->emissionAngle(sB)) > 90.) {
        shape->clearSurfacePoint();
        shape->setHasIntersection(false);
        return false;
      }
    }

    // return with success
    shape->setHasIntersection(true);
    
    return true;
  }



  /**
  * Returns the look direction in the camera coordinate system
  *
  * @param v[] The look vector
  */
  void Sensor::LookDirection(double v[3]) const {
    vector<double> lookB(3);
    lookB[0] = m_lookB[0];
    lookB[1] = m_lookB[1];
    lookB[2] = m_lookB[2];
    vector<double> lookJ = bodyRotation()->J2000Vector(lookB);
    vector<double> lookC = instrumentRotation()->ReferenceVector(lookJ);
    v[0] = lookC[0];
    v[1] = lookC[1];
    v[2] = lookC[2];
  }

  /**
   * Returns the right ascension angle (sky longitude)
   */
  double Sensor::RightAscension() {
    if (m_newLookB) computeRaDec();
    return m_ra;
  }

  /**
   * Returns the declination angle (sky latitude)
   * @return @b double Declination angle.
   */
  double Sensor::Declination() {
    if (m_newLookB) computeRaDec();
    return m_dec;
  }

  /**
   * Protected method which computes the ra/dec of the current look direction
   */
  void Sensor::computeRaDec() {
    m_newLookB = false;
    vector<double> lookB(3);
    lookB[0] = m_lookB[0];
    lookB[1] = m_lookB[1];
    lookB[2] = m_lookB[2];
    vector<double> lookJ = bodyRotation()->J2000Vector(lookB);;

    SpiceDouble range;
    recrad_c((SpiceDouble *)&lookJ[0], &range, &m_ra, &m_dec);
    m_ra *= 180.0 / PI;
    m_dec *= 180.0 / PI;
  }

  /**
   * Given the ra/dec compute the look direction
   *
   * @param ra    Right ascension in degrees (sky longitude)
   * @param dec   Declination in degrees (sky latitude)
   *
   * @return @b bool True if successful
   */
  bool Sensor::SetRightAscensionDeclination(const double ra, const double dec) {
    vector<double> lookJ(3);
    radrec_c(1.0, ra * PI / 180.0, dec * PI / 180.0, (SpiceDouble *)&lookJ[0]);

    vector<double> lookC = instrumentRotation()->ReferenceVector(lookJ);
    return SetLookDirection((double *)&lookC[0]);
  }


  /**
   * Return the vector between the spacecraft and surface point in body-fixed
   *
   * @author 2011-12-20 Tracie Sucharski
   */
  void Sensor::SpacecraftSurfaceVector(double scSurfaceVector[3]) const {
    scSurfaceVector[0] = m_lookB[0];
    scSurfaceVector[1] = m_lookB[1];
    scSurfaceVector[2] = m_lookB[2];
  }



  /**
   * Return the distance between the spacecraft and surface point in kmv
   * @return @b double Slant distance
   */
  double Sensor::SlantDistance() const {
    SpiceDouble psB[3], upsB[3];
    SpiceDouble dist;

    std::vector<double> sB = bodyRotation()->ReferenceVector(instrumentPosition()->Coordinate());

    SpiceDouble pB[3];
    ShapeModel *shape = target()->shape();
    pB[0] = shape->surfaceIntersection()->GetX().kilometers();
    pB[1] = shape->surfaceIntersection()->GetY().kilometers();
    pB[2] = shape->surfaceIntersection()->GetZ().kilometers();

    vsub_c(pB, (SpiceDouble *) &sB[0], psB);
    unorm_c(psB, upsB, &dist);
    return dist;
  }

  /**
   * Return the local solar time in hours
   * @return @b double Local solar time, in hours
   */
  double Sensor::LocalSolarTime() {
    double slat, slon;
    subSolarPoint(slat, slon);

    double lst = UniversalLongitude() - slon + 180.0;
    lst = lst / 15.0;  // 15 degress per hour
    if (lst < 0.0) lst += 24.0;
    if (lst > 24.0) lst -= 24.0;
    return lst;
  }

  /**
   * Returns the distance between the sun and surface point in AU
   * @return @b double Solar distance
   */
  double Sensor::SolarDistance() const {
    // Get the sun coord
    double sB[3];
    Spice::sunPosition(sB);

    // Calc the change
    ShapeModel *shape = target()->shape();
    double xChange = sB[0] - shape->surfaceIntersection()->GetX().kilometers();
    double yChange = sB[1] - shape->surfaceIntersection()->GetY().kilometers();
    double zChange = sB[2] - shape->surfaceIntersection()->GetZ().kilometers();

    // Calc the distance and convert to AU
    double dist = sqrt(xChange*xChange + yChange*yChange + zChange*zChange);
    dist /= 149597870.691;
    return dist;
  }


  /**
   * Returns the distance from the spacecraft to the subspacecraft point in km.
   * It uses the ellipsoid, not the shape model
   * @return @b double Spacecraft altitude.
   */
  double Sensor::SpacecraftAltitude() {
    // Get the spacecraft coord
    double spB[3];
    Spice::instrumentPosition(spB);

    // Get subspacecraft point
    double lat, lon;
    subSpacecraftPoint(lat, lon);
    double rlat = lat * PI / 180.0;
    double rlon = lon * PI / 180.0;

    // Compute radius
    Distance rad = LocalRadius(lat, lon);

    // Now with the 3 spherical value compute the x/y/z coordinate
    double ssB[3];
    latrec_c(rad.kilometers(), rlon, rlat, ssB);

    // Calc the change
    double xChange = spB[0] - ssB[0];
    double yChange = spB[1] - ssB[1];
    double zChange = spB[2] - ssB[2];

    // Calc the distance
    double dist = sqrt(xChange*xChange + yChange*yChange + zChange*zChange);
    return dist;
  }


  /**
   * Gets the radius from the DEM, if we have one.

   * @return @b double Local radius from the DEM
   */
//  Distance Sensor::DemRadius(const SurfacePoint &pt) {
//    return DemRadius(pt.GetLatitude(), pt.GetLongitude());
//  }


  /**
   * Gets the radius from the DEM, if we have one.
   * @param lat Latitude
   * @param lon Longitude
   * @return @b double Local radius from the DEM
   */
//  Distance Sensor::DemRadius(const Latitude &lat, const Longitude &lon) {
//    if (!m_hasElevationModel) return Distance();
//    //if (!lat.Valid() || !lon.Valid()) return Distance();
//    m_demProj->SetUniversalGround(lat.degrees(), lon.degrees());
//    if (!m_demProj->IsGood()) {
//      return Distance();
//    }

//    m_portal->SetPosition(m_demProj->WorldX(), m_demProj->WorldY(), 1);

//    m_demCube->read(*m_portal);

//    const double &radius = m_interp->Interpolate(m_demProj->WorldX(),
//                                                 m_demProj->WorldY(),
//                                                 m_portal->DoubleBuffer());

//    return Distance(radius, Distance::Meters);
//      Distance fred;
//      return fred;
//  }


  /**
   * This method is an optimized version of DemRadius(Latitude, Longitude) meant
   * for SetLookDirection until the Projection class has been refactored to use
   * Latitude and Longitude instead of doubles.
   *
   * Please do not call this method as it is lacking checks that
   *   SetLookDirection has already performed.
   *
   * @returns A radius in kilometers
   */
//  double Sensor::DemRadius(double lat, double lon) {
//    if (!m_demProj->SetUniversalGround(lat, lon)) {
//      return Isis::Null;
//    }

//    m_portal->SetPosition(m_demProj->WorldX(), m_demProj->WorldY(), 1);
//    m_demCube->read(*m_portal);

//    double radius = m_interp->Interpolate(m_demProj->WorldX(),
//                                          m_demProj->WorldY(),
//                                          m_portal->DoubleBuffer());
//    if (Isis::IsSpecial(radius)) {
//      return Isis::Null;
//    }

//    return radius / 1000.0;
//      double fred;
//      return fred;
//  }
  /**
   * Indicates whether the Kernels PvlGroup has an ElevationModel or
   * ShapeModel PvlKeyword value.
   *
   * @return @b bool True if an elevation model exists.
   */
//   bool HasElevationModel() {
//     return m_hasElevationModel;
//   };

}
