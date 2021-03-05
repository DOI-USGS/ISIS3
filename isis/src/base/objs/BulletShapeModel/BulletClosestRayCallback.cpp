/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>

#include <QtGlobal>
#include <QScopedPointer>
#include <QString>
#include <QVector>

#include "BulletClosestRayCallback.h"
#include "BulletTargetShape.h"
#include "Constants.h"
#include "IException.h"
#include "IsisBullet.h"
#include "SurfacePoint.h"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

namespace Isis {


  /**
   * Default constructor. The ray beginning and end, intersection point, and
   * normal default to the origin. The triangle index and part ID default to -1.
   */
  BulletClosestRayCallback::BulletClosestRayCallback() : 
    btCollisionWorld::ClosestRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)), 
                            m_point(0.0, 0.0, 0.0), m_normal(0.0, 0.0, 0.0),
                            m_triangleIndex(-1), m_partId(-1) {

    // Set default ray tracing flags
    m_flags = defaultFlags();
  }


  /**
   * Construct a callback from another callback, an intersection point, and a
   * surface normal.
   * 
   * @param result The callback to to construct a copy of. The intersection
   *               point and normal will not be copied.
   * @param point The intersection point to store.
   * @param normal The surface normal to store.
   */
  BulletClosestRayCallback::BulletClosestRayCallback(const BulletClosestRayCallback &result, 
                                                     const btVector3 &point,
                                                     const btVector3 &normal) :
                            btCollisionWorld::ClosestRayResultCallback(result.observer(), result.lookdir()) {
    *this = result;
    m_point = point;
    m_normal = normal;
    return;
  }


  /**
   * Construct a callback from a ray start and end.
   * 
   * @param observer The beginning of the ray.
   * @param lookdir The end of the ray.
   */
  BulletClosestRayCallback::BulletClosestRayCallback(const btVector3 &observer, 
                                                     const btVector3 &lookdir) : 
                            btCollisionWorld::ClosestRayResultCallback(observer, lookdir), 
                            m_point(0.0, 0.0, 0.0), m_normal(0.0, 0.0, 0.0),
                            m_triangleIndex(-1), m_partId(-1) { 

    // Set default ray tracing flags
    m_flags = defaultFlags();
  }


  /**
   * @brief This constructor is used to create a single ray hit 
   *  
   * This constructor is intended to be used in a multi-ray hit environment such 
   * as the BulletAllRayHitsRayCallback object to provide the same features 
   * provided in this single hit object. 
   * 
   * @author 2017-03-17 Kris Becker 
   * 
   * @param observer The beginning of the ray.
   * @param lookdir The end of the ray.
   * @param source The original callback this is being created from.
   * @param rayResult The local intersection result.
   * @param normalInWorldSpace Is the normal in the local result in the local coordinate system
   *                           or the world coordinate system?
   */
  BulletClosestRayCallback::BulletClosestRayCallback(const btVector3 &observer, 
                                                     const btVector3 &lookdir,
                                                     const btCollisionWorld::RayResultCallback &source, 
                                                     btCollisionWorld::LocalRayResult &rayResult, 
                                                     bool normalInWorldSpace) :
                            btCollisionWorld::ClosestRayResultCallback(observer, lookdir), 
                            m_point(0.0, 0.0, 0.0), m_normal(0.0, 0.0, 0.0),
                            m_triangleIndex(-1), m_partId(-1)  {
     copyRayResult(*this, source);
     addSingleResult(rayResult, normalInWorldSpace);
  }


  /**
   * Destory a callback.
   */
  BulletClosestRayCallback::~BulletClosestRayCallback() { }


  /**
   * Checks if the callback is valid/has a valid intersection.
   * 
   * @return @b bool If there is a valid intersection.
   * 
   * @see ClosestRayResultCallback::hasHit()
   */
  bool BulletClosestRayCallback::isValid() const {
    return ( hasHit() );
  }


  /**
   * Return the beginning of the ray.
   * 
   * @return @b btVector3 The beginning of the ray.
   */
  btVector3 BulletClosestRayCallback::observer() const {
    return ( ClosestRayResultCallback::m_rayFromWorld );
  }


  /**
   * Return the end of the ray.
   * 
   * @return @b btVector3 The end of the ray.
   */
  btVector3 BulletClosestRayCallback::lookdir() const {
    return ( ClosestRayResultCallback::m_rayToWorld );
  }


  /**
   * Return the intersection hit fraction or fractional distance along the ray
   * of the intersection. This is where along the ray the intersection was
   * found. If the hit fraction is 0, then the intersection was found at the
   * very beginning of the ray. If the hit fraction is 1, then the intersection
   * was found at the very end of the ray. The intersection point can be
   * calculates as:
   * 
   * (1 - hitFraction) * rayStart + hitFraction * rayEnd
   * 
   * @return @btScalar The closest intersection hit fraction.
   */
  btScalar BulletClosestRayCallback::fraction() const {
    return ( RayResultCallback::m_closestHitFraction );
  }


  /**
   * Return the intersection point, if one exists.
   * 
   * @return @b btVector3 The intersection point in body fixed (x, y, z) kilometers.
   */
  btVector3 BulletClosestRayCallback::point() const {
    if ( hasHit() ) { 
      return ( m_point );
    }
    throw IException(IException::Programmer, 
                     "No hits in ray trace so no surface point!", 
                     _FILEINFO_);
    return ( btVector3(0.0, 0.0, 0.0) );
  }


  /**
   * Return the local surface normal at the intersection, if an intersection exists.
   * 
   * @return @b btVector3 The local surface normal in body fixed (x, y, z).
   */
  btVector3 BulletClosestRayCallback::normal() const {
    if ( hasHit() ) {
      return ( m_normal );
    }

    throw IException(IException::Programmer, 
                     "No hits in ray trace so no normal!", 
                     _FILEINFO_);
    return btVector3(0.0, 0.0, 0.0);
  }


  /**
   * Return the 0-based index of the intersected triangle.
   * 
   * @return @b int The 0-based index of the intersected triangle.
   */
  int BulletClosestRayCallback::triangleIndex() const {
    return ( m_triangleIndex );
  }


  /**
   * Return the Bullet ID of the intersected collision object.
   * 
   * @return @b int The Bullet ID of the intersected collision object.
   */
  int BulletClosestRayCallback::partId() const {
    return ( m_partId );
  }


  /** 
   * Returns the distance from the intersection point to the beginning of the ray.
   * 
   * @return @b btScalar Distance between observer and point in kilometers. If
   *                     no intersection exists, then DBL_MAX is returned.
   */
  btScalar BulletClosestRayCallback::distance() const {
    if ( !isValid() ) return (DBL_MAX);
    return ( observer().distance(point()) );
  }


  /** 
   * Returns the distance from the intersection point of this callback to the
   * intersection point of another callback.
   *  
   * @param other The other callback to find the distance to.
   * 
   * @return @b btScalar Distance between the intersection point and the other
   *                     callback's intersection point in kilometers. If this
   *                     callback or the other callback does not have an
   *                     intersection, then DBL_MAX is returned.
   */
  btScalar BulletClosestRayCallback::distance(const BulletClosestRayCallback &other) const {
    if ( !(isValid() && other.isValid()) ) return (DBL_MAX);
    return ( point().distance( other.point() ));
  }


  /** 
   * Returns the distance from the intersection point to another point.
   * 
   * @param other The point to calculate the distance to.
   * 
   * @return @b btScalar Distance between intersection and point in kilometers.
   *                     If no intersection exists, then DBL_MAX is returned.
   */
  btScalar BulletClosestRayCallback::distance(const btVector3 &other) const {
    if ( !isValid() ) return (DBL_MAX);
    return ( point().distance(other) );
  }


  /**
   * Equality operator to check if this callback is equivalent to another callback.
   * 
   * @param other The callback to compare with.
   * 
   * @return @b bool If both callbacks are valid and intersected the same
   *                 triangle on the same collision body.
   */
  bool BulletClosestRayCallback::operator==(const BulletClosestRayCallback &other) const {
    if ( !(isValid() && other.isValid()) ) return (false);
    if ( triangleIndex() != other.triangleIndex() ) return (false);
    if ( partId() != other.partId() ) return (false);
    return ( true );
  }


  //TODO This needs a better name
  /**
   * Check if the intersection in this is visible based on another callback.
   * Three things are checked to confirm visibility. First, both callbacks
   * must intersect the same triangle on the same collision object. Second, the
   * angle between this callback's local normal and the vector from the other
   * callback's beginning to this callback's intersection must be less than
   * or equal to 90 degrees. Finally, the distance between the intersections of
   * the two callbacks must be less than the tolerance.
   * 
   * @param other The callback for checking visibility. This should represent
   *              the observer's look ray.
   * @param tolerance The tolerance for the final check of the distance between
   *                  intersection points.
   * 
   * @return @b bool If the intersection in this callback is visible.
   */
  bool BulletClosestRayCallback::isVisible(const BulletClosestRayCallback &other,
                                            const btScalar tolerance) const {
    if ( !(isValid() && other.isValid()) ) return (false);
    if ( triangleIndex() != other.triangleIndex() ) return (false);
    if ( partId() != other.partId() ) return (false);
    
    // How close are the two intercept points?
    if ( distance( other ) > tolerance ) return ( false );

    return (true);  // Its visable!
  }

/**
 * @breif Return pointer to target shape 
 *  
 * This method assumes the creators have properly set the user pointer in (at 
 * least) the btCollisionObject. 
 * 
 * @author 2017-03-17 Kris Becker
 * 
 * @return const BulletClosestRayCallback::BulletTargetShape* 
 */
  const BulletTargetShape *BulletClosestRayCallback::body() const {
    if ( !isValid() ) return (0);
    return ( static_cast<const BulletTargetShape *> (m_collisionObject->getUserPointer()) );
  }


  /**
   * Add a local intersection result to this callback during ray casting.
   * Because this callback  only stores the single closest result, this
   * overwrites any currently stored result.
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
  btScalar BulletClosestRayCallback::addSingleResult(btCollisionWorld::LocalRayResult &rayResult,
                                                     bool normalInWorldSpace) {
    btScalar hitFraction = ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);

    m_point = m_hitPointWorld;
    m_normal = m_hitNormalWorld;

    // Triangle information
    m_triangleIndex = rayResult.m_localShapeInfo->m_triangleIndex;
    m_partId        = rayResult.m_localShapeInfo->m_shapePart;
    return (hitFraction);
  }


  /**
   * Return the default ray cast flags. The flags come from the EFlags
   * enumeration in the Bullet class btTriangleRaycastCallback.
   * 
   * @return @b int The bitwise default flags.
   */
   unsigned int BulletClosestRayCallback::defaultFlags() const {
     return ( (btTriangleRaycastCallback::kF_FilterBackfaces | 
               btTriangleRaycastCallback::kF_KeepUnflippedNormal |
               btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest) );
   }

  /**
   * Easy way to copy one callback into another.
   * 
   * @param dest The callback to copy over.
   * @param source The callback to copy from.
   */
  void BulletClosestRayCallback::copyRayResult(btCollisionWorld::RayResultCallback &dest, 
                                               const btCollisionWorld::RayResultCallback &source) {
    dest = source;
    return;
  }

} // namespace Isis



