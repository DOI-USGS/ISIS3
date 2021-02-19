#ifndef EmbreeShapeModel_h
#define EmbreeShapeModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ShapeModel.h"

#include <vector>

#include <embree2/rtcore.h>

#include <QMap>
#include <QVector>

#include "Distance.h"
#include "EmbreeTargetShape.h"
#include "EmbreeTargetManager.h"
#include "SurfacePoint.h"
#include "Target.h"

namespace Isis {
  class Target;

  /**
   * @brief General purpose Embree ray tracing model
   *
   * @author 2017-04-22 Jesse Mapel and Jeannie Backer
   *
   * @internal
   *   @history 2017-04-22 Jesse Mapel and Jeannie Backer - Original Version
   *   @history 2018-05-01 Christopher Combs - Removed emissionAngle function to
   *                fix issues with using ellipsoids to find normals. Fixes #5387.
   */
  class EmbreeShapeModel : public ShapeModel {
    public:
      // Constructors
      EmbreeShapeModel();
      EmbreeShapeModel(Target *target, Pvl &pvl,
                       EmbreeTargetManager *targetManager);
      EmbreeShapeModel(Target *target, const QString &shapefile,
                       EmbreeTargetManager *targetManager);

      // Destructor
      virtual ~EmbreeShapeModel();

      // Intersect the shape model
      virtual bool intersectSurface(std::vector<double> observerPos,
                                    std::vector<double> lookDirection);
      virtual bool intersectSurface(const Latitude &lat, const Longitude &lon,
                                    const std::vector<double> &observerPos,
                                    const bool &backCheck = true);
      virtual bool intersectSurface(const SurfacePoint &surfpt, 
                                    const std::vector<double> &observerPos,
                                    const bool &backCheck = true);

      virtual void clearSurfacePoint();

      virtual bool isDEM() const;

      double getTolerance() const;
      void   setTolerance(const double &tolerance);

      // Calculate the default normal of the current intersection point
      virtual void calculateDefaultNormal();
      // Calculate the surface normal of the current intersection point
      virtual void calculateLocalNormal(QVector<double *> cornerNeighborPoints);
      virtual void calculateSurfaceNormal();
      QVector<double> ellipsoidNormal();

      virtual double incidenceAngle(const std::vector<double> &uB);

      virtual Distance localRadius(const Latitude &lat, const Longitude &lon);

      // Determine if the internal intercept is occluded from the observer/lookdir
      virtual bool isVisibleFrom(const std::vector<double> observerPos,
                                 const std::vector<double> lookDirection);

    private:
      // Disallow copying because ShapeModel is not copyable
      Q_DISABLE_COPY(EmbreeShapeModel)

      void updateIntersection(const RayHitInformation hitInfo);
      RTCMultiHitRay latlonToRay(const Latitude &lat, const Longitude &lon) const;
      RTCMultiHitRay pointToRay(const  SurfacePoint &point) const;
      QVector< RayHitInformation > sortHits(RTCMultiHitRay &ray,
                                            LinearAlgebra::Vector &observer);

      EmbreeTargetShape   *m_targetShape;   /**!< The target body and Embree objects
                                                  for intersection. This is owned and
                                                  managed by the target manager and
                                                  will be deleted by that. */
      EmbreeTargetManager *m_targetManager; /**!< This manages EmbreeTargetShapes to
                                                  allow for sharing of them between
                                                  EmbreeShapeModels and deletes them
                                                  when no longer needed. */
      double               m_tolerance;     /**!< Tolerance for checking visibility. */
      QString              m_shapeFile;     /**!< The shapefile used to create the target
                                                  shape. */
  };
};

#endif
