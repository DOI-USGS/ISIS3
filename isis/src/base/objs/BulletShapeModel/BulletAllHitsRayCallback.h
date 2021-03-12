#ifndef BulletAllHitsRayCallback_h
#define BulletAllHitsRayCallback_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QScopedPointer>
#include <QString>
#include <QVector>


#include "BulletClosestRayCallback.h"
#include "Constants.h"
#include "IException.h"
#include "SurfacePoint.h"

namespace Isis {


/**
 * Bullet ray tracing callback to return all intersections along a ray's path.
 * 
 * @author 2017-03-17 Kris Becker 
 * @internal 
 *   @history 2017-03-17  Kris Becker  Original Version
 */
  class BulletAllHitsRayCallback : public btCollisionWorld::AllHitsRayResultCallback {
    public:
      BulletAllHitsRayCallback();
      BulletAllHitsRayCallback(const btVector3 &observer, const btVector3 &lookdir,
                               const bool cullBackfacers = true);
      virtual ~BulletAllHitsRayCallback();

      bool isValid() const;
      int size() const;

      btVector3 observer() const;
      btVector3 lookdir() const;

      const BulletClosestRayCallback &hit(const int &index = 0) const;

    protected:
      QVector<BulletClosestRayCallback> m_rayHits;  //!< List of ray hits

      virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult,
                                       bool normalInWorldSpace);
  };

} // namespace Isis

#endif

