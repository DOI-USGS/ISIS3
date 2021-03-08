#ifndef EllipsoidShape_h
#define EllipsoidShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ShapeModel.h"

template<class T> class QVector;

namespace Isis {
  class Pvl;

  /**
   * @brief Define shapes and provide utilities for ISIS targets
   *
   * This class will define shapes of ISIS target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @ingroup
   *
   * @author 2010-08-02 Debbie A. Cook
   *
   * @internal
   *   @history 2012-12-21 Debbie A. Cook - Cleaned up intersectSurface method to
   *                           reflect changes made to ShapeModel class.  References #1343.
   *   @history 2015-04-30 Jeannie Backer - Added isDEM() method. References #2243.
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                           were signaled. References #2248.
   *   @history 2016-06-13 Kelvin Rodriguez - Removed redundant contructor
   *                           EllipsoidShape(Target, Pvl). References #2214
   *   @history 2017-05-19 Tyler Wilson - calculateSurfaceNormal() and calculateDefaultNormal()
   *                           now call calculateLocalNormal(), which calculates the normal
   *                           vector to an ellipsoid.  Prior to this, they were calling
   *                           ShapeModel::calculateEllipsoidalSurfaceNormal() function
   *                           which was incorrectly returning the normal vector to a sphere and not
   *                           an ellipsoid.  Fixes #1028.
   *   @history 2017-06-07 Kristin Berry - Added a using declaration so that the new
   *                            intersectSurface methods in ShapeModel are accessible by
   *                            EllipsoidShape.
   */
  class EllipsoidShape : public Isis::ShapeModel {
    public:
      //! Constructors
      EllipsoidShape(Target *target);
      EllipsoidShape();

      //! Destructor
      ~EllipsoidShape() { };

      // Make parent functions visible
      using Isis::ShapeModel::intersectSurface;

      //! Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

      //! Calculate the default normal of the current intersection point
      virtual void calculateDefaultNormal();

      // implement pure virtual method from ShapeModel class
      bool isDEM() const;

      //! Calculate the local surface normal of the current intersection point
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints);

      //! Calculate the surface normal of the current intersection point
      void calculateSurfaceNormal();

      //! Get the local radius for a point on the surface
      Distance localRadius(const Latitude &lat, const Longitude &lon);

    private:
  };
};

#endif
