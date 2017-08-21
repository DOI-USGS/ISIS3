#ifndef BulletShapeModel_h
#define BulletShapeModel_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
