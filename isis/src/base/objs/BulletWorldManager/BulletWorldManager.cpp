/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
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
  BulletWorldManager::BulletWorldManager() :  m_data( new BulletWorldData() ) {

  }


  /**
   * Construct a world manager with a given name.
   * 
   * @param name The name of the world.
   */
  BulletWorldManager::BulletWorldManager(const QString &name) : 
                                         m_data( new BulletWorldData(name) ) {

  }

  /**
   * Construct a world from an existing world. This creates a shared instance. To 
   * build a custom world you use the default constructor and add shapes. 
   */
  BulletWorldManager::BulletWorldManager(const BulletWorldManager &world) :
                                         m_data(world.m_data) {
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
    return ( m_data->m_name );
  }


  /**
   * Number of collision objects in the world.
   * 
   * @return @b int The number of collision objects in the world.
   */
  int BulletWorldManager::size() const{
    btAssert( !m_data->m_world.isNull() );
    return ( m_data->m_world->getCollisionObjectArray().size() );
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
    btAssert( !m_data->m_world.isNull() );
    return ( (BulletTargetShape *) (m_data->m_world->getCollisionObjectArray().at(index)->getUserPointer()) );
  }


  /**
   * Look for a specific collision object by name
   * 
   * @param name Name of collision object to search for
   * 
   * @return @b BulletTargetShape* Pointer to shape if found. Otherwise null.
   */
  BulletTargetShape *BulletWorldManager::getTarget(const QString &name) const { 
    btAssert( !m_data->m_world.isNull() );
    QString v_name = name.toLower();
    const btCollisionObjectArray &btobjects = m_data->m_world->getCollisionObjectArray();
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


    //  Ensure validity
    btAssert( !m_data->m_mutex.isNull() );
    btAssert( !m_data->m_world.isNull() );

    // May need to retain the target in a list for world destruction!!??
    // CollisionBody is expected to contain a UserPointer that links back to 
    // target (BulletTargetShape or some other type). 
    QMutexLocker locker(m_data->m_mutex.data());
    m_data->m_world->addCollisionObject( target->body() );
    m_data->m_world->updateAabbs();
    return;
  }


/**
 * @brief Perform ray casting from a position and a look direction 
 *  
 * Its not clear if mutex locking is needed here for several reasons. Bullet is 
 * thread safe but only if it was compiled with BULLET2_USE_THREAD_LOCKS=ON. The 
 * default is to not turn on thread locking so invoke thread locking here. 
 * However, if its built with threading, it is not clear if thread locking is
 * needed and should to be tested. 
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

    //  Ensure validity
    btAssert( !m_data->m_mutex.isNull() );
    btAssert( !m_data->m_world.isNull() );

    QMutexLocker locker(m_data->m_mutex.data());
    m_data->m_world->rayTest(rayStart, rayEnd, results);
    return ( results.hasHit() );
  }


  /**
   * Get the Collision World where the targets exist
   * 
   * @return @b btCollisionWorld The Bullet collision world used for ray casting.
   */
  const btCollisionWorld &BulletWorldManager::getWorld() const {
    btAssert( !m_data->m_world.isNull() );
    return ( *(m_data->m_world.data()) );
  }


}  // namespace Isis
