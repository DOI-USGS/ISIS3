#ifndef EquatorialCylindricalShape_h
#define EquatorialCylindricalShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "DemShape.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Define shapes and provide utilities for shapes stored as ISIS EquatorialCylindrical map
   *
   * This class will define shapes of ISIS target bodies with the shape defined by an
   * EquatorialCylindrical map, as well as provide utilities to retrieve radii and photometric
   * information for the intersection point.
   *
   * @author 2010-07-30 Debbie A. Cook
   *
   * @internal
   *   @history 2012-12-19 Debbie A. Cook Added a check for an invalid radius in the
   *                           intersectSurface() method, that was causing applications to hang.
   *                           Also return a false if no ellipsoid itersection is found.
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                           were signaled. References #2248.
   *   @history 2015-10-01 Jeannie Backer - Made improvements to documentation and brought code
   *                           closer to ISIS coding standards. Removed check for ellipsoid
   *                           intersection in intersectSurface() method to prevent early return
   *                           and attempt the iterative method even when the ellipsoid is not
   *                           intersected. Fixes #1438
   *   @history 2018-01-05 Cole Neubauer - Fixed units conversion in intersectSurface so that the
   *                           loop is stepping by radians per pixel, as recommended by Jeff
   *                           Anderson (LROC team). Fixes #5245
   */
  class EquatorialCylindricalShape : public DemShape {
    public:
      // Constructor
      EquatorialCylindricalShape(Target *target, Pvl &pvl);

      // Destructor
      ~EquatorialCylindricalShape();

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

    private:
      Distance *m_minRadius;  //!< Minimum radius value in DEM file
      Distance *m_maxRadius;  //!< Maximum radius value in DEM file
  };
};

#endif
