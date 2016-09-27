#include "EllipsoidShape.h"

#include <QVector>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Distance.h"
#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "ShapeModel.h"
#include "SurfacePoint.h"

using namespace std;

namespace Isis {

  /**
   * Initialize the EllipsoidShape.
   *
   * @param pvl Valid Isis3 cube label.
   */
  EllipsoidShape::EllipsoidShape(Target *target) : ShapeModel (target) {
    setName("Ellipsoid");
  }


  /**
   * Initialize the EllipsoidShape.
   *
   * @param pvl Valid Isis3 cube label.
   */
  EllipsoidShape::EllipsoidShape() : ShapeModel () {
    setName("Ellipsoid");
  }


  /** Find the intersection point
   *
   */
  bool EllipsoidShape::intersectSurface (std::vector<double> observerPos,
                                         std::vector<double> lookDirection) {

    return (intersectEllipsoid(observerPos, lookDirection));
  }


  /** Calculate default normal
   *
   */
  void EllipsoidShape::calculateDefaultNormal()  {
    calculateEllipsoidalSurfaceNormal();
  }


  /** Calculate surface normal
   *
   */
  void EllipsoidShape::calculateSurfaceNormal()  {
    calculateEllipsoidalSurfaceNormal();
  }


  /**
   * Indicates that this shape model is not from a DEM. Since this method
   * returns false for this class, the Camera class will not calculate the
   * local normal using neighbor points.
   *
   * @return bool Indicates that this is not a DEM shape model.
   */
  bool EllipsoidShape::isDEM() const {
    return false;
  }


  /** Calculate local normal
   *
   */
  void EllipsoidShape::calculateLocalNormal(QVector<double *> cornerNeighborPoints)  {

    if (!surfaceIntersection()->Valid() || !hasIntersection()) {
     IString msg = "A valid intersection must be defined before computing the surface normal";
      throw IException(IException::Programmer, msg, _FILEINFO_);
   }

   // Get the coordinates of the current surface point
    SpiceDouble pB[3];
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Get the radii of the ellipsoid
    vector<Distance> radii = targetRadii();
    double a = radii[0].kilometers();
    double b = radii[1].kilometers();
    double c = radii[2].kilometers();
    vector<double> normal(3,0.);

    NaifStatus::CheckErrors();
    surfnm_c(a, b, c, pB, (SpiceDouble *) &normal[0]);
    NaifStatus::CheckErrors();

    setNormal(normal);
    setHasNormal(true);
  }


  /**
   * Gets the local radius for the given latitude/longitude coordinate.
   *
   * @return Distance The distance from the center of the ellipsoid to its
   *         surface at the given lat/lon location.
   *
   */
  Distance EllipsoidShape::localRadius(const Latitude &lat, const Longitude &lon) {

    std::vector<Distance> radii = targetRadii();

    double a = radii[0].kilometers();
    double b = radii[1].kilometers();
    double c = radii[2].kilometers();

    double rlat = lat.radians();
    double rlon = lon.radians();

    double xyradius = a * b / sqrt(pow(b * cos(rlon), 2) +
                      pow(a * sin(rlon), 2));
    const double &radius = xyradius * c / sqrt(pow(c * cos(rlat), 2) +
                           pow(xyradius * sin(rlat), 2));

    return Distance(radius, Distance::Kilometers);
  }

}
