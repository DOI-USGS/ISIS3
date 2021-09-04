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

#include "BulletTargetShape.h"
#include "BulletDskShape.h"

#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Pvl.h"

namespace Isis {

  /**
   * @brief Default empty constructor.
   * 
   * The filename defaults to an empty string and the maximum distance defaults to 0.
   */
  BulletTargetShape::BulletTargetShape()
    : m_maximumDistance(0) { }


  /** 
   * Constructs a BulletTargetShape for a given a Bullet object
   * 
   * @param btbody The Bullet collision object to contain.
   * @param name The name of the object.
   */
  BulletTargetShape::BulletTargetShape(btCollisionObject *btbody, const QString &name) :
                                       m_name(name), m_btbody(btbody) {
    setMaximumDistance();
  }


  /**
   * Desctructor
   */
  BulletTargetShape::~BulletTargetShape() { }


  /**
   * Calculate and save the maximum distance across the body. This is
   * calculated as the distance from the x, y, z minimum to x, y, z maximum.
   */
  void BulletTargetShape::setMaximumDistance() {
    if (m_btbody) {
      btVector3 center;
      m_btbody->getCollisionShape()->getBoundingSphere(center, m_maximumDistance);
      m_maximumDistance *= 2;
    }
    else {
      m_maximumDistance = 0;
    }
  }


  /**
   * Return name of the target shape
   * 
   * @return @b QString The target name
   */
  QString BulletTargetShape::name() const {
    return ( m_name );
  }


  /**
   * Load a DEM file into the target shape.
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::load(NaifContextPtr naif, const QString &dem, const Pvl *conf) {
    FileName v_file(dem);
    
    QString ext = v_file.extension().toLower();

    if ( "bds" == ext) return ( loadDSK(naif, dem) );
    if ( "cub" == ext) return ( loadCube(dem) );
    return ( loadPC(dem) );
  }


  /** 
   * Load a point cloud type DEM in Bullet.
   * 
   * @note Currently not implemented
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::loadPC(const QString &dem, const Pvl *conf) {
    return (0);
  }

  /**
   * Load a DSK in Bullet
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::loadDSK(NaifContextPtr naif, const QString &dem, const Pvl *conf) {
    return ( new BulletDskShape(naif, dem) );
  }

  /** Load an ISIS cube type DEM in Bullet.
   * 
   * @note Currently not implemented
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::loadCube(const QString &dem, const Pvl *conf) {
    return (0);
  }

  /** 
   * Write a serialized version of the target shape to a Bullet file
   * 
   * @param btName The name of the file to write the target shape to.
   * 
   * @note Currently not implemented
   */
  void BulletTargetShape::writeBullet(const QString &btName) const {

  }

  /** 
   * Return a pointer to the Bullet target object/shape
   * 
   * @return @b btCollisionObject A target to the Bullet collision object.
   */
  btCollisionObject *BulletTargetShape::body() const {
    return ( m_btbody.data() );
  }


  /** Set the Bullet shape object to this object instance   */
  void BulletTargetShape::setTargetBody(btCollisionObject *body) {
    m_btbody.reset(body);
    m_btbody->setUserPointer(this);
    setMaximumDistance();
    return;
  }

  btScalar BulletTargetShape::maximumDistance() const {
    return m_maximumDistance;
  }

}  // namespace Isis
