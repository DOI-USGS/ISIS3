#ifndef BulletWorldManager_h
#define BulletWorldManager_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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

