/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IsisShape.h"

#include <QVector>

#include "ShapeModel.h"
#include "SurfacePoint.h"

using namespace std;

namespace Isis {
  /**
   * Create an IsisShape that wraps an ISIS ShapeModel.
   */
  IsisShape::IsisShape (ShapeModel* shape) {
    m_shape = shape;
  }


  /**
   * Intersect the ShapeModel.
   * Depending on what type of ShapeModel this is wrapping, the local normal will
   * be computed differently. Of note, DEM ShapeModels do not support local normal
   * calculations right now, because of the extra information required by the
   * current ISIS DEM local normal calculation requiring extra observer rays.
   */
  SensorUtilities::Intersection IsisShape::intersect(const SensorUtilities::Vec &sensorPos, const SensorUtilities::Vec &lookVec, bool computeLocalNormal) {
    vector<double> pos = {sensorPos.x, sensorPos.y, sensorPos.z};
    vector<double> look = {lookVec.x, lookVec.y, lookVec.z};
    m_shape->intersectSurface(pos, look);
    SurfacePoint *intersection = m_shape->surfaceIntersection();
    if (computeLocalNormal && !m_shape->isDEM()) {
      QVector<double *> unusedNeighborPoints(4);
      double origin[3] = {0, 0, 0};
      unusedNeighborPoints.fill(origin);
      m_shape->calculateLocalNormal(unusedNeighborPoints);
    }
    else {
      m_shape->calculateSurfaceNormal();
    }
    vector<double> normal = m_shape->normal();
    return {
          {intersection->GetX().meters(), intersection->GetY().meters(), intersection->GetZ().meters()},
          {normal[0], normal[1], normal[2]}};
  }
}