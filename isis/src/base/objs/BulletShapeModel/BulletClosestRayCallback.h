#ifndef BulletClosestRayCallback_h
#define BulletClosestRayCallback_h
/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/08/25 01:37:55 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QScopedPointer>
#include <QString>
#include <QVector>

#include "Constants.h"
#include "IException.h"
#include "IsisBullet.h"
#include "SurfacePoint.h"

namespace Isis {

class BulletTargetShape;

/**
 * Bullet ray tracing callback for closest hit on target surface. Stores the
 * results of a raycast to find the closest intersection to the start of a ray.
 * If no raycast has been preformed, or a raycast was unsuccessful, then
 * the callback will be flagged as invalid.
 * 
 * @author 2017-03-17 Kris Becker 
 * @internal 
 *   @history 2017-03-17  Kris Becker  Original Version
 */
  class BulletClosestRayCallback : public btCollisionWorld::ClosestRayResultCallback {
    public:
      BulletClosestRayCallback();
      BulletClosestRayCallback(const BulletClosestRayCallback &result, const btVector3 &point,
                               const btVector3 &normal);
      BulletClosestRayCallback(const btVector3 &observer, const btVector3 &lookdir);
      BulletClosestRayCallback(const btVector3 &observer, const btVector3 &lookdir,
                               const btCollisionWorld::RayResultCallback &source, 
                               btCollisionWorld::LocalRayResult &rayResult, 
                               bool nornmalInWorldSpace);
      virtual ~BulletClosestRayCallback();

      bool isValid() const;

      btVector3 observer() const;
      btVector3 lookdir() const;

      btScalar  fraction() const;
      btVector3 point() const;
      btVector3 normal() const;
      int       triangleIndex() const;
      int       partId() const;

      btScalar distance() const;
      btScalar distance(const BulletClosestRayCallback &other) const;
      btScalar distance(const btVector3 &other) const;
      bool operator==(const BulletClosestRayCallback &other) const;

      bool isVisible(const BulletClosestRayCallback &other,
                      const btScalar tolerance = DBL_MAX) const;

      const BulletTargetShape *body() const;

    protected:
      btVector3 m_point;         /**! The intersection point in body fixed (x, y, z) kilometers. */
      btVector3 m_normal;        /**! The local surface normal at the intersection point in
                                      body fixed (x, y, z). */
      int       m_triangleIndex; /**! The 0-based index of the intersected triangle. */
      int       m_partId;        /**! The Bullet ID of the intersected collision object. */
      

      virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, 
                                       bool normalInWorldSpace);

    private:
      unsigned int defaultFlags() const;
      void copyRayResult(btCollisionWorld::RayResultCallback &dest, 
                         const btCollisionWorld::RayResultCallback &source);

  };

} // namespace Isis

#endif

