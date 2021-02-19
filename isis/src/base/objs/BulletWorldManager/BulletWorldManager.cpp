/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BulletWorldManager.h"

#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>

#include <QMutexLocker>

#include "FileName.h"
#include "IException.h"
#include "IString.h"

using namespace std;

namespace Isis {

  /**
   * Default empty constructor.
   */
  BulletWorldManager::BulletWorldManager() {
    m_name = "Body-Fixed-Coordinate-System";
    initWorld();  
  }


  /**
   * Construct a world manager with a given name.
   * 
   * @param name The name of the world.
   */
  BulletWorldManager::BulletWorldManager(const QString &name) {
    m_name = name;
    initWorld();  
  }


  /**
   * Destroys the BulletWorldManager.
   */
  BulletWorldManager::~BulletWorldManager() { }


  /**
   * Name of the world
   * 
   * @return @b QString The name of the Bullet collision world.
   */
  QString BulletWorldManager::name() const {
    return ( m_name );
  }


  /**
   * Number of collision objects in the world.
   * 
   * @return @b int The number of collision objects in the world.
   */
  int BulletWorldManager::size() const{
    return ( m_world->getCollisionObjectArray().size() );
  }


  /**
   * Return a collision object by index into the world
   * 
   * @param index Index from 0 to size()-1 of the object that exist in the world
   * 
   * @return @b BulletTargetShape* Pointer to shape if found.
   */
  BulletTargetShape *BulletWorldManager::getTarget(const int &index) const {
    btAssert( index < size() );
    btAssert( index >= 0 );
    return ( (BulletTargetShape *) (m_world->getCollisionObjectArray().at(index)->getUserPointer()) );
  }


  /**
   * Look for a specific collision object by name
   * 
   * @param name Name of collision object to search for
   * 
   * @return @b BulletTargetShape* Pointer to shape if found. Otherwise null.
   */
  BulletTargetShape *BulletWorldManager::getTarget(const QString &name) const { 

    QString v_name = name.toLower();
    const btCollisionObjectArray &btobjects = m_world->getCollisionObjectArray();
    for ( int i = 0 ; i < btobjects.size() ; i++ ) {
      BulletTargetShape *target = (BulletTargetShape *)  (btobjects[i]->getUserPointer());
      if ( target->name().toLower() == v_name ) {
        return (target);
      }
    }
   
    // Not found...
    return (0);
  }


/**
 * Add a Bullet shape to the collision world
 * 
 * @param target The target shape to add to the world.
 */
  void BulletWorldManager::addTarget(BulletTargetShape *target) {

    // May need to retain the target in a list for world destruction!!??
    // CollisionBody is expected to contain a UserPointer that links back to 
    // target (BulletTargetShape or some other type). 
    m_world->addCollisionObject( target->body() );

  }


/**
 * @brief Perform ray casting from a position and a look direction
 * 
 * @author 2017-03-17 Kris Becker 
 * 
 * @param         rayStart The origin of the ray
 * @param         rayEnd   The end point of the ray  
 * @param[in,out] results  Ray intersection callback. holds the output results
 *                         of the ray cast. The type of callback determines
 *                         what happens when an intersection is found during
 *                         ray casting.
 * 
 * @return @b bool Returns true if any intersections are detected
 * 
 * @see btCollisionWorld::rayTest
 */
  bool BulletWorldManager::raycast(const btVector3 &rayStart, const btVector3 &rayEnd, 
                                   btCollisionWorld::RayResultCallback &results ) const {
    m_world->rayTest(rayStart, rayEnd, results);
    return ( results.hasHit() );
  }


  /**
   * Get the Collision World where the targets exist
   * 
   * @return @b btCollisionWorld The Bullet collision world used for ray casting.
   */
  const btCollisionWorld &BulletWorldManager::getWorld() const {
    return ( *m_world );
  }


/**
 * Initialize the collision world for object ray tracing 
 * 
 * @author  2017-03-17 Kris Becker 
 */
  void BulletWorldManager::initWorld() {
    m_collision.reset( new btDefaultCollisionConfiguration() );
    m_dispatcher.reset(new btCollisionDispatcher(m_collision.data()) );
    m_broadphase.reset( new btDbvtBroadphase() );  // Could also be an AxisSweep
    m_world.reset( new btCollisionWorld( m_dispatcher.data(), m_broadphase.data(), m_collision.data() ) );

  }

}  // namespace Isis
