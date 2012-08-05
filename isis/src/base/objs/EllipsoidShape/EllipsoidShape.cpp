#include <algorithm>
#include <cfloat>
#include <string>
#include <vector>

#include <cmath>
#include <iomanip>

#include "EllipsoidShape.h"

#include "Distance.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "ShapeModel.h"
#include "SurfacePoint.h"

namespace Isis {
  /**
   * Initialize the EllipsoidShape.
   *
   * @param pvl Valid Isis3 cube label.
   */
  EllipsoidShape::EllipsoidShape(Target *target, Pvl &pvl) : ShapeModel (target, pvl) {
    std::cout << "Making ellipsoid shape" << std::endl;
    setName("Ellipsoid");
  }


  /**
   * Initialize the EllipsoidShape.
   *
   * @param pvl Valid Isis3 cube label.
   */
  // EllipsoidShape::EllipsoidShape(Distance radii[3]) : ShapeModel (radii) {
  EllipsoidShape::EllipsoidShape(Target *target) : ShapeModel (target) {
    std::cout << "Making ellipsoid shape" << std::endl;
    setName("Ellipsoid");
  }
  
  
  /** Find the intersection point
   *
   */
  bool EllipsoidShape::intersectSurface (std::vector<double> observerPos,
                                         std::vector<double> lookDirection) {
    
      if (intersectEllipsoid(observerPos, lookDirection)) {
      m_hasIntersection = true;
      return m_hasIntersection;
    }
    else {
      m_hasIntersection = false;
      return false;
    }
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


  /** Calculate local normal
   *
   */
  void EllipsoidShape::calculateLocalNormal(QVector<double *> cornerNeighborPoints)  {
    calculateEllipsoidalSurfaceNormal();
  }


  /** get local radius
   *
   */
  Distance EllipsoidShape::localRadius(const Latitude &lat, const Longitude &lon) {
    
    // Distance *radii = radii();
    
    Distance *radii = targetRadii();
    
    double a = radii[0].kilometers();
    double b = radii[1].kilometers();
    double c = radii[2].kilometers();
    
    double rlat = lat.degrees();
    double rlon = lon.degrees();
    
    double xyradius = a * b / sqrt(pow(b * cos(rlon), 2) +
                      pow(a * sin(rlon), 2));
    
    const double &radius = xyradius * c / sqrt(pow(c * cos(rlat), 2) +
                           pow(xyradius * sin(rlat), 2));

    return Distance(radius, Distance::Kilometers);
  }
}
