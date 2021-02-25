#ifndef BulletDskShape_h
#define BulletDskShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QScopedPointer>
#include <QString>
#include <QVector>

#include "BulletTargetShape.h"
#include "BulletClosestRayCallback.h"

namespace Isis {

/**
 * Bullet Target Shape for NAIF type 2 DSK models
 *
 * @author 2017-03-17 Kris Becker
 * @internal
 *   @history 2017-03-17  Kris Becker  Original Version
 */
  class BulletDskShape : public BulletTargetShape {
    public:
      BulletDskShape();
      BulletDskShape(const QString &dskfile);
      virtual ~BulletDskShape();

      int getNumTriangles() const;
      int getNumVertices() const;

      virtual btVector3 getNormal(const int indexId, const int segment=0) const;
      virtual btMatrix3x3 getTriangle(const int index, const int segment=0) const;

    private:
      QSharedPointer<btTriangleIndexVertexArray> m_mesh; /**! Triangular mesh representation of
                                                              the target shape. The vertex ordering
                                                              is the same as in the DSK file,
                                                              except the DSK uses 1-based indexing
                                                              and this uses 0-based indexing. */

      // Custom DSK reader
      void loadFromDsk(const QString &dskfile);

  };

} // namespace Isis

#endif
