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



