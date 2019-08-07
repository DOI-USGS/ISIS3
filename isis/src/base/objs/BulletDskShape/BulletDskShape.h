#ifndef BulletDskShape_h
#define BulletDskShape_h
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
