#ifndef PlaneShape_h
#define PlaneShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */#include "ShapeModel.h"

#include <string>
#include <vector>

#include <QVector>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Pvl.h"
#include "SurfacePoint.h"

namespace Isis {
  /**
   * @brief Define plane shape model
   *
   * This class defines a plane shape model for ISIS target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @ingroup
   *
   * @author 2012-07-30 Ken Edmundson
   *
   * @internal
   *   @history 2015-04-30 Jeannie Backer - Added isDEM() method. References #2243.
   *                           Moved method implementation to cpp file.
   *   @history 2015-07-31 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF
   *                           errors were signaled. References #2248.
   *   @history 2016-06-13 Kelvin Rodriguez - Removed redundant contructor PlaneShape(Target, Pvl).
   *                           References #2214
   *   @history 2017-06-07 Kristin Berry - Added a using declaration so that the new
   *                           intersectSurface methods in ShapeModel are accessible by DemShape.
   */
  class PlaneShape : public Isis::ShapeModel {
    public:
      // Constructors
      PlaneShape(Target *target, Isis::Pvl &pvl);
      PlaneShape(Target *target);
      PlaneShape();

      // Destructor
      ~PlaneShape();

      // Make parent functions visible
      using Isis::ShapeModel::intersectSurface;

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

      bool isDEM() const;

      // Calculate the surface normal of the current intersection point
      void calculateSurfaceNormal();
      void calculateDefaultNormal();
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints);

      double emissionAngle(const std::vector<double> & sB);
      double incidenceAngle(const std::vector<double> &uB);

      // Get the local radius for a point on the surface
      Distance localRadius(const Latitude &lat, const Longitude &lon);
  };
};

#endif
