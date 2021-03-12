/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>

#include <QScopedPointer>
#include <QString>
#include <QVector>


#include "BulletAllHitsRayCallback.h"
#include "Constants.h"
#include "IException.h"
#include "SurfacePoint.h"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

namespace Isis {


  /**
   * Default constructor. The ray beginning and end default to the origin.
   * The intersections vector defaults to empty.
   */
  BulletAllHitsRayCallback::BulletAllHitsRayCallback() : 
    btCollisionWorld::AllHitsRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)),
    m_rayHits() {
    m_flags = (btTriangleRaycastCallback::kF_KeepUnflippedNormal |
               btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest);
  }


  /**
   * Construct a callback with a ray start and ray end.
   * 
   * @param observer The beginning of the ray.
   * @param lookdir The end of the ray.
   * @param cullBackfacers If back facing intersections should be culled.
   */
  BulletAllHitsRayCallback::BulletAllHitsRayCallback(const btVector3 &observer, 
                                                     const btVector3 &lookdir,
                                                     const bool cullBackfacers) : 
                            btCollisionWorld::AllHitsRayResultCallback(observer, lookdir),
                            m_rayHits() {
    m_flags = (btTriangleRaycastCallback::kF_KeepUnflippedNormal |
               btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest);
    if ( cullBackfacers ) {
      m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
    }
  }


  /**
   * Destroy this callback.
   */
  BulletAllHitsRayCallback::~BulletAllHitsRayCallback() { }


  /**
   * Checks if the callback is valid/has a valid intersection.
   * 
   * @return @b bool If there is a valid intersection.
   * 
   * @see ClosestRayResultCallback::hasHit()
   */
  bool BulletAllHitsRayCallback::isValid() const {
    return ( hasHit() );
  }


  /**
   * Returns the number of intersections found.
   * 
   * @return @b int The number of stored intersections in this callback.
   */
  int BulletAllHitsRayCallback::size() const {
    return ( m_rayHits.size() );
  }


  /**
   * Return the beginning of the ray.
   * 
   * @return @b btVector3 The beginning of the ray.
   */
  btVector3 BulletAllHitsRayCallback::observer() const {
    return ( AllHitsRayResultCallback::m_rayFromWorld );
  }


  /**
   * Return the end of the ray.
   * 
   * @return @b btVector3 The end of the ray.
   */
  btVector3 BulletAllHitsRayCallback::lookdir() const {
    return ( AllHitsRayResultCallback::m_rayToWorld );
  }


  /**
   * Return a callback for the intersection at a given index.
   * 
   * @param index The index of the intersection.
   * 
   * @return @b BulletClosestRayCallback The callback for the intersection
   */
  const BulletClosestRayCallback &BulletAllHitsRayCallback::hit(const int &index) const {
    btAssert( index >= 0 );
    btAssert( index < size() );
    return ( m_rayHits[index]);
  }


  /**
   * Add a local intersection result to this callback during ray casting.
   * 
   * @note This method is called automatically by btCollisionWorld::rayTest.
   * 
   * @param rayResult The local intersection result for the ray cast.
   * @param normalInWorldSpace If the normal stored in the local result is in
   *                           local coordinates or world coordinates.
   * 
   * @return @b btScalar The hit fraction, fractional distance along the ray,
   *                     of the intersections
   */
  btScalar BulletAllHitsRayCallback::addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace) {
    btCollisionWorld::AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
    m_rayHits.push_back(BulletClosestRayCallback(observer(), lookdir(), *this, rayResult, normalInWorldSpace));
    return (rayResult.m_hitFraction);
  }
} // namespace Isis



