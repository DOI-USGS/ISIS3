#ifndef IsisShape_h
#define IsisShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SensorUtilities.h"

namespace Isis {
  class ShapeModel;

  /**
   * Implementation of SensorUtilities::Shape backed by an ISIS ShapeModel.
   */
  class IsisShape : SensorUtilities::Shape {
    public:
      IsisShape(ShapeModel* shape);

      virtual SensorUtilities::Intersection intersect(const SensorUtilities::Vec &sensorPos, const SensorUtilities::Vec &lookVec, bool computeLocalNormal=true);
    private:
      ShapeModel* m_shape;
  };
};

#endif