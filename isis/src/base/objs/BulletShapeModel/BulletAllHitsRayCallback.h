#ifndef BulletAllHitsRayCallback_h
#define BulletAllHitsRayCallback_h
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

