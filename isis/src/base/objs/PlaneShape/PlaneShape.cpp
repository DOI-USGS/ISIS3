/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <algorithm>
#include <cfloat>
#include <string>
#include <vector>

#include <cmath>
#include <iomanip>

#include "PlaneShape.h"

#include "Distance.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "ShapeModel.h"
#include "SurfacePoint.h"

namespace Isis {
  /**
   * Initialize the PlaneShape.
   *
   * @param pvl Valid ISIS cube label.
   */
  PlaneShape::PlaneShape(Target *target, Pvl &pvl) : ShapeModel (target) {
    setName("Plane");

    // set surface normal
    // setNormal(0.0, 0.0, 1.0);
  }


  /**
   * Initialize the PlaneShape.
   *
   * @param pvl Valid ISIS cube label.
   */
  PlaneShape::PlaneShape(Target *target) : ShapeModel (target) {
    setName("Plane");

    // set normal vector
    // setNormal(0.0, 0.0, 1.0);
  }


  /**
   * Initialize the PlaneShape.
   *
   * @param pvl Valid ISIS cube label.
   */
  PlaneShape::PlaneShape() : ShapeModel () {
    setName("Plane");
  }


  /**
   * Destructor
   */
  PlaneShape::~PlaneShape() {
  }


  /** Find the intersection point
   *
   * @param observerPos: observer (likely a spacecraft) position in Body-Fixed
   * coordinates.
   *
   * @param lookDirection: observer (likely a spacecraft) look vector in Body-
   * Fixed coordinates.
   */
  bool PlaneShape::intersectSurface (std::vector<double> observerPos,
                                     std::vector<double> lookDirection) {
    NaifStatus::CheckErrors();
    SpiceDouble zvec[3];
    SpicePlane plane;
    SpiceInt nxpts;
    SpiceDouble xpt[3];

//    std::vector<double> n = normal();

//    zvec[0] = n[0];
//    zvec[1] = n[1];
//    zvec[2] = n[2];
    zvec[0] = 0.0;
    zvec[1] = 0.0;
    zvec[2] = 1.0;

    if (observerPos[2] < 0.0)
      zvec[2] = -zvec[2];

    // NAIF routine to "Make a CSPICE plane from a normal vector and a constant"
    // see http://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/nvc2pl_c.html
    nvc2pl_c(zvec, 0.0, &plane);

    SpiceDouble position[3];
    SpiceDouble lookvector[3];

    position[0] = observerPos[0];
    position[1] = observerPos[1];
    position[2] = observerPos[2];

    lookvector[0] = lookDirection[0];
    lookvector[1] = lookDirection[1];
    lookvector[2] = lookDirection[2];

    // NAIF routine to "find the intersection of a ray and a plane"
    // see http://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/inrypl_c.html
    inrypl_c(&position, &lookvector, &plane, &nxpts, xpt);

    if (nxpts != 1 ) {
      setHasIntersection(false);
      return false;
    }

    setHasIntersection(true);
    setNormal(0.0,0.0,1.0);
    surfaceIntersection()->FromNaifArray(xpt);
    NaifStatus::CheckErrors();

    return true;
  }


  /**
   * Indicates that this shape model is not from a DEM. Since this method
   * returns false for this class, the Camera class will not calculate the
   * local normal using neighbor points.
   *
   * @return bool Indicates that this is not a DEM shape model.
   */
  bool PlaneShape::isDEM() const {
    return false;
  }


  /**
   * There is no implementation for this method.
   */
  void PlaneShape::calculateSurfaceNormal() {
  }


  /**
   * There is no implementation for this method.
   */
  void PlaneShape::calculateDefaultNormal() {
  }


  /**
   * There is no implementation for this method.
   */
  void PlaneShape::calculateLocalNormal(QVector<double *> cornerNeighborPoints) {
  }


  /**
   * Computes and returns emission angle in degrees given the observer position.
   *
   * Emission Angle: The angle between the surface normal vector at the
   * intersection point and a vector from the intersection point to the
   * spacecraft. The emission angle varies from 0 degrees when the spacecraft is
   * viewing the sub-spacecraft point (nadir viewing) to 90 degrees when the
   * intercept is tangent to the surface of the target body. Thus, higher values
   * of emission angle indicate more oblique viewing of the target.
   *
   * @param sB: Spacecraft position in body-fixed coordinates
   *
   * @return Emmision angle in decimal degrees
   *
   */
  double PlaneShape::emissionAngle(const std::vector<double> & sB) {

    SpiceDouble pB[3];   // surface intersection in body-fixed coordinates
    SpiceDouble psB[3];  // vector from spacecraft to surface intersection
    SpiceDouble upsB[3]; // unit vector from spacecraft to surface intersection
    SpiceDouble dist;    // vector magnitude

    // Get vector from center of body to surface point
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Get vector from surface intersect point to observer and normalize it
    vsub_c((ConstSpiceDouble *) &sB[0], pB, psB);
    unorm_c(psB, upsB, &dist);

    // temporary normal vector
    SpiceDouble n[3];
    n[0] = 0.0;
    n[1] = 0.0;
    n[2] = 1.0;

    // flip normal if observer is "below" the plane, assuming that the target
    // body north pole defines the "up" direction
    if (sB[2] < 0.0)
      n[2] = -n[2];

    // dot product of surface normal and observer-surface intersection vector
    double angle = vdot_c(n, upsB);

    if (angle > 1.0)
      return 0.0;

    if (angle < -1.0)
      return 180.0;

    return acos(angle) * RAD2DEG;
  }


  /**
   * Computes and returns incidence angle in degrees given the sun position.
   *
   * Incidence Angle: The incidence angle provides a measure of the lighting
   * condition at the surface intersection point. The angle between the surface
   * normal vector at the intersection point and a vector from the intersection
   * point to the sun. The incidence angle varies from 0 degrees when the
   * intersection point coincides with the sub-solar point to 90 degrees when
   * the intersection point is at the terminator (i.e., in the shadowed or dark
   * portion of the target body). Thus, higher values of incidence angles
   * indicate the existence of a greater number of surface shadows.
   *
   * @param uB: Sun position in body-fixed coordinates
   *
   * @return Incidence angle in decimal degrees
   *
   */
  double PlaneShape::incidenceAngle(const std::vector<double> &uB) {

    SpiceDouble pB[3];   // surface intersection in body-fixed coordinates
    SpiceDouble puB[3];  // vector from sun to surface intersection
    SpiceDouble upuB[3]; // unit vector from sun to surface intersection
    SpiceDouble dist;    // vector magnitude

    // Get vector from center of body to surface point
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Get vector from surface intersect point to sun and normalize it
    vsub_c((SpiceDouble *) &uB[0], pB, puB);
    unorm_c(puB, upuB, &dist);

    // temporary normal vector
    SpiceDouble n[3];
    n[0] = 0.0;
    n[1] = 0.0;
    n[2] = 1.0;

    // flip normal if sun is "below" the plane, assuming that the target
    // body north pole defines the "up" direction
    if (uB[2] < 0.0)
      n[2] = -n[2];

    double angle = vdot_c((SpiceDouble *) &n[0], upuB);

    if (angle > 1.0)
      return 0.0;

    if(angle < -1.0)
      return 180.0;

    return acos(angle) * RAD2DEG;
  }


  /**
   * Gets the local radius for the given latitude/longitude coordinate. For the
   * plane shape model, this is calculated by finding the distance of the
   * surface intersection point from the plane's origin.
   *
   * @return Distance The distance from the center of the body to its surface at
   *         the given lat/lon location.
   *
   */
  // TODO: what should this do in the case of a ring plane (or any other plane
  // for that matter)?
  Distance PlaneShape::localRadius(const Latitude &lat, const Longitude &lon) {

    SpiceDouble pB[3];   // surface intersection in body-fixed coordinates

    // Get vector from center of body to surface point
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    double radius = sqrt(pB[0]*pB[0] + pB[1]*pB[1] + pB[2]*pB[2]);

    return Distance(radius, Distance::Kilometers);
  }
}
