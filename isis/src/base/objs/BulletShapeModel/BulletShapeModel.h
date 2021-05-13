#ifndef BulletShapeModel_h
#define BulletShapeModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ShapeModel.h"

#include <vector>

#include <QList>
#include <QMap>

#include "Intercept.h"
#include "BulletAllHitsRayCallback.h"
#include "BulletClosestRayCallback.h"
#include "BulletTargetShape.h"
#include "BulletWorldManager.h"
#include "Target.h"

namespace Isis {
  class Target;

  /**
   * Shape model that uses the Bullet library to perform ray tracing.
   *
   * @author 2017-03-22 Kris Becker
   *
   * @internal
   *   @history 2017-03-22 - Kris Becker - Original Version
 
   */
  class BulletShapeModel : public ShapeModel {
    public:
      // Constructors
      BulletShapeModel();
      BulletShapeModel(Target *target, Pvl &pvl);
      BulletShapeModel(const QString &shapefile, Target *target, Pvl &pvl);
      BulletShapeModel(BulletTargetShape *shape, Target *target, Pvl &pvl);
      BulletShapeModel(BulletWorldManager *model, Target *target, Pvl &pvl);

      // Destructor
      ~BulletShapeModel();

      double getTolerance() const;
      void   setTolerance(const double &tolerance);

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);
      virtual bool intersectSurface(const Latitude &lat, const Longitude &lon,
                                    const std::vector<double> &observerPos,
                                    const bool &checkOcclusion = true);
      virtual bool intersectSurface(const SurfacePoint &surfpt, 
                                    const std::vector<double> &observerPos,
                                    const bool &checkOcclusion = true);

      virtual void setSurfacePoint(const SurfacePoint &surfacePoint);
      virtual void clearSurfacePoint();

      // Calculate the default normal of the current intersection point
      void calculateDefaultNormal();

      bool isDEM() const;

      // Calculate the surface normal of the current intersection point
      void setLocalNormalFromIntercept();
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints);
      void calculateSurfaceNormal();

      Distance localRadius(const Latitude &lat, const Longitude &lon);

      QVector<double> ellipsoidNormal();

      const BulletWorldManager &model() const;


      // Determine if the internal intercept is occluded from the observer/lookdir
      virtual bool isVisibleFrom(const std::vector<double> observerPos,
                                 const std::vector<double> lookDirection);

    private:
      // Disallow copying because ShapeModel is not copyable
      Q_DISABLE_COPY(BulletShapeModel)

      QScopedPointer<BulletWorldManager> m_model;        /**! Bullet collision world that contains
                                                              the representation of the body. */
      double                             m_tolerance;    /**! Tolerance of occlusion check in
                                                              kilometers. */
      BulletClosestRayCallback           m_intercept;    /**! The results of the last ray cast. */

      btScalar maxDistance() const;

      btVector3 castLookDir(const btVector3 &observer, const btVector3 &lookdir) const;
      btVector3 latlonToVector(const Latitude &lat, const Longitude &lon) const;
      btVector3 pointToVector(const  SurfacePoint &point) const;
      SurfacePoint makeSurfacePoint(const btVector3 &point) const;

      QVector<BulletClosestRayCallback> sortHits(const BulletAllHitsRayCallback &hits,
                                                 const btVector3 &sortPoint) const;
      bool isOccluded(const BulletClosestRayCallback &hit,
                      const btVector3 &observer) const;

      void updateShapeModel(const BulletClosestRayCallback &result);


  };
}

#endif
