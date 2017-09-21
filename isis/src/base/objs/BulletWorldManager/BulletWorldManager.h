#ifndef BulletWorldManager_h
#define BulletWorldManager_h
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

#include <QCache>
#include <QMutex>
#include <QScopedPointer>
#include <QString>
#include <QtGlobal>

#include "IsisBullet.h"
#include "BulletTargetShape.h"\

namespace Isis {

/**
 * @brief Bullet World manager maintains a proper state for target bodies
 *  
 * This class maintains the physics world created in Bullet-verse. This world is 
 * target-centric in that the center of the world is the center of the 
 * target (body-fixed) coordinate system. This could be changed to represent 
 * some other system (e.g., J2000), However this design allows one to operate on 
 * a per target basis. 
 *  
 * This class uses only the collision body world of Bullet, which is static and
 * will not be subject to simulation operations. 
 *  
 * This design also allows for a whole target body DEM to be loaded or a large 
 * body broken up into smaller parts of the whole if needed. 
 *  
 * @author 2017-03-17 Kris Becker 
 *  
 * @internal 
 *   @history 2017-03-17  Kris Becker  Original Version
 */
  class BulletWorldManager {
    public:
      BulletWorldManager();
      BulletWorldManager(const QString &name);
      virtual ~BulletWorldManager();

      QString name() const;
      int size() const;

      BulletTargetShape *getTarget(const int &index = 0) const;
      BulletTargetShape *getTarget(const QString &name) const;
      void addTarget(BulletTargetShape *target);

      bool raycast( const btVector3 &observer, const btVector3 &lookdir, 
                    btCollisionWorld::RayResultCallback &hits ) const;

      const btCollisionWorld &getWorld() const;


    private:
      Q_DISABLE_COPY(BulletWorldManager)

      QString                                         m_name; /**! The name of the Bullet
                                                                   world. */
      QScopedPointer<btDefaultCollisionConfiguration> m_collision; /**! The collision
                                                                        configuration for
                                                                        the world. */
      QScopedPointer<btCollisionDispatcher>           m_dispatcher; /**! The dispatcher for the
                                                                         world. */
      QScopedPointer<btBroadphaseInterface>           m_broadphase; /**! The interface for overlaps
                                                                         in the world's aabb
                                                                         acceleration tree. */
      QScopedPointer<btCollisionWorld>                m_world; /**! The Bullet collision world that
                                                                    contains the representation of
                                                                    the body. */

      mutable QMutex m_mutex;       //!< Mutex for thread safety

      void initWorld();

  };

} // namespace Isis

#endif

