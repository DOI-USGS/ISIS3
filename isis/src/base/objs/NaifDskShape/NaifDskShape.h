#ifndef NaifDskShape_h
#define NaifDskShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ShapeModel.h"

#include <vector>

#include <QVector>

#include "Intercept.h"
#include "NaifDskPlateModel.h"

namespace Isis {
  class Intercept;

  /**
   * @brief Provides support for NAIF's Digital Shape Kernel (DSK)
   *
   * @author 2014-02-07 Kris Becker
   *
   * @internal
   *   @history 20104-02-07 - Kris Becker - Original Version
   *   @history 2015-03-08 Jeannie Backer - Changed name from  NaifDskShapeModel to match other
   *                           classes derived from ShapeModel. Added documentation and test.
   *                           Added class to ISIS trunk. References #2035
   *   @history 2015-03-14 Jeannie Backer - Modified calculateLocalNormal() to simply call
   *                           setLocalNormalFromIntercept() rather than trying to
   *                           interpolate based on the neighbor values. References #2035
   *   @history 2015-04-30 Jeannie Backer - Added isDEM() method. References #2243.
   *   @history 2016-06-13 Kelvin Rodriguez - Removed redundant contructor PlaneShape(Target, Pvl).
   *                           References #2214
   *   @history 2017-06-07 Kristin Berry - Added a using declaration so that the new 
   *                            intersectSurface methods in ShapeModel are accessible by
   *                            EllipsoidShape.
   *   @todo Remove Model from name to match other derived classes
   */
  class NaifDskShape : public ShapeModel {
    public:
      // Constructors
      NaifDskShape();
      NaifDskShape(Target *target, Pvl &pvl);
      NaifDskShape(const NaifDskPlateModel &model);

      // Destructor
      ~NaifDskShape();

      // Make parent functions visible
      using Isis::ShapeModel::intersectSurface;

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

      bool intersectSurface(const SurfacePoint &surfpt,
                            const std::vector<double> &observerPos,
                            const bool &backCheck=true);

      // Calculate the default normal of the current intersection point
      void calculateDefaultNormal();

      bool isDEM() const;

      // Calculate the surface normal of the current intersection point
      void setLocalNormalFromIntercept();
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints);
      void calculateSurfaceNormal();

      Distance localRadius(const Latitude &lat, const Longitude &lon);

      QVector<double> ellipsoidNormal();

      const NaifDskPlateModel &model() const;
      const Intercept *intercept() const;


    private:
      // Disallow copying because ShapeModel is not copyable
      NaifDskShape(const NaifDskShape &model);
      NaifDskShape &operator=(const NaifDskShape &model);

      NaifDskPlateModel         m_model;     //!< Plate model to intersect.
      QScopedPointer<Intercept> m_intercept; //!< Pointer to the shape's intercept.


  };
}

#endif
