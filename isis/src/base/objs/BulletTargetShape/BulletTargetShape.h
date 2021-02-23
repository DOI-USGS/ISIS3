#ifndef BulletTargetShape_h
#define BulletTargetShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QSharedPointer>
#include <QScopedPointer>
#include <QString>

#include "IsisBullet.h"
#include "BulletClosestRayCallback.h"

namespace Isis {


  class Pvl; 

/**
 * @brief Bullet Target Shape for planetary bodies 
 *  
 * This class contains the in memory representation of a body for use with the
 * Bullet library. For each type of file that can be used to create a bullet
 * target body, this class should be extended to manage that type of file.
 * 
 * @author 2017-03-17 Kris Becker 
 * @internal 
 *   @history 2017-03-17  Kris Becker  Original Version
 */
  class BulletTargetShape {
    public:
      BulletTargetShape();
      BulletTargetShape(btCollisionObject *btbody, const QString &name = "");
      virtual ~BulletTargetShape();

      QString name() const;

      // Special constructors
      static BulletTargetShape *load(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadPC(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadDSK(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadCube(const QString &dem, const Pvl *conf = 0);

      void writeBullet(const QString &btName) const;
      btCollisionObject *body() const;

      btScalar maximumDistance() const;

    protected:
      void setTargetBody(btCollisionObject *body);
      void setMaximumDistance();

    private:
      QString                           m_name; /**! The name of the body */
      QSharedPointer<btCollisionObject> m_btbody; /**! The Bullet collision object
                                                       for the body */
      btScalar                          m_maximumDistance; /**! The distance from the minimum
                                                                x, y, z values to the maximum
                                                                x, y, z values. */

  };

} // namespace Isis

#endif

