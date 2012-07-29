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
  EllipsoidShape::EllipsoidShape(Pvl &pvl) : ShapeModel (pvl) {
    std::cout << "Making ellipsoid shape" << std::endl;
    setName("Ellipsoid");
  }


  /**
   * Initialize the EllipsoidShape.
   *
   * @param pvl Valid Isis3 cube label.
   */
  EllipsoidShape::EllipsoidShape(Distance radii[3]) : ShapeModel (radii) {
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


  /** Calculate surface normal
   *
   */
  void EllipsoidShape::calculateSurfaceNormal() {
    // The below code is not truly normal unless the ellipsoid is a sphere.  TODO Should this be
    // fixed? Send an email asking Jeff and Stuart.  See Naif routine surfnm.c to get the true 
    // for an ellipsoid.  For testing purposes to match old results do as Isis3 currently does until
    // Jeff and Stuart respond.

   if ( !m_hasIntersection ) {
     Isis::iString msg = "A valid intersection must be defined before computing the surface normal";
      throw IException(IException::Programmer, msg, _FILEINFO_);
   }
    SpiceDouble pB[3];
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Unitize the vector
    SpiceDouble upB[3];
    SpiceDouble dist;
    unorm_c(pB, upB, &dist);
    memcpy(&m_surfaceNormal[0], upB, sizeof(double) * 3);
    m_hasNormal = true;
  }


  /** get local radius
   *
   */
  Distance EllipsoidShape::localRadius(const Latitude &lat, const Longitude &lon) {
    
    Distance *radii = targetRadii();
    
//    radii = targetRadii();
    
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
