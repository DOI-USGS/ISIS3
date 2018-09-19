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
#include <QExplicitlySharedDataPointer>
#include <QObject>
#include <QSharedData>
#include <QScopedPointer>
#include <QString>
#include <QtGlobal>

#include "IsisBullet.h"
#include "BulletTargetShape.h"

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
 * will not be subject to simulation iterations. 
 *  
 * This design also allows for a whole target body DEM to be loaded or a large 
 * body broken up into smaller parts of the whole if needed. Secondarily, this 
 * class creates a BulletTargetShape which is automatically added to the world 
 * as its fundamental target. 
 *  
 * This class has been made to be thread safe and explicitly shared using Qt's 
 * sharing features. Note this implementation does not support copying the 
 * shared BulletWorldData structure (it is ill-defined because of how Bullet 
 * manages data pointer structures). Because of this, an explicit share only of 
 * the data is provided while forbidding copying (detaching) the data. This is 
 * enforced by use of scoped pointers and explicit use of Q_DISABLE_COPY. 
 * However, copying of BulletWorldManager is encouraged! 
 *  
 * @author 2017-03-17 Kris Becker 
 *  
 * @internal 
 *   @history 2017-03-17  Kris Becker  Original Version
 *   @history 2018-07-21 UA/OSIRIS-REx IPWG Team - Made this a completely
 *                         shareable object with a thread safe implementation
 *   @history 2018-09-13 UA/OSIRIS-REx IPWG Team - Modified to use a more
 *                         efficient shared object data strategy; improved
 *                         implementation and documentation
 */
  class BulletWorldManager {
    public:
      BulletWorldManager();
      BulletWorldManager(const QString &name);
      BulletWorldManager(const BulletWorldManager &world);
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

    /**
     * Wrapper for Bullet World data 
     *  
     * @see QSharedData 
     *  
     * @author 2018-09-13 UA/OSIRIS-REx IPWG Team - Original Version
     */
      class BulletWorldData : public QSharedData {
        public:
          /** Basic initialization   */
          BulletWorldData() : QSharedData()  {
              initWorld();
          }

          /** Create with a nammed world   */
          BulletWorldData(const QString &name) : QSharedData()  {
              initWorld(name);
          }

          /** Destructor   */
          ~BulletWorldData() { }

          QString                                         m_name; /**! The name of the Bullet
                                                                       world. */
          // Order of these pointers matter due to destructor behavior!
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
          QScopedPointer<QMutex>                          m_mutex; //!< Mutex for thread safety

        private:
          Q_DISABLE_COPY(BulletWorldData);  // Copy is undefined for this context

          /** Initialize a new Bullet world structure   */
          void initWorld(const QString &name = "Body-Fixed-Coordinate-System") { 
            m_name = name;   
            m_collision.reset( new btDefaultCollisionConfiguration() );
            m_dispatcher.reset(new btCollisionDispatcher( m_collision.data() ) );
            m_broadphase.reset( new btDbvtBroadphase() );  // Could also be an AxisSweep
            m_world.reset( new btCollisionWorld( m_dispatcher.data(), 
                                                 m_broadphase.data(), 
                                                 m_collision.data() ) );
            m_mutex.reset( new QMutex() );
          }
      };

      QExplicitlySharedDataPointer<BulletWorldData> m_data; //!< Shared data to Bullet world

  };


} // namespace Isis

//  Improves Qt container performance by notifying Qt's subsystem of its
//  movable nature
Q_DECLARE_TYPEINFO(Isis::BulletWorldManager, Q_MOVABLE_TYPE);
#endif

