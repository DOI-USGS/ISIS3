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

#include <QExplicitlySharedDataPointer>
#include <QObject>
#include <QSharedData>
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
 *   @history 2018-07-21 UA/OSIRIS-REx IPWG Team  - Add filename() method to
 *                          return name of DSK file associated with this object
 */
  class BulletDskShape : public BulletTargetShape {
    public:
      BulletDskShape();
      BulletDskShape(const QString &dskfile);
      virtual ~BulletDskShape();

      QString filename() const;
      int getNumTriangles() const;
      int getNumVertices() const;

      virtual btVector3 getNormal(const int index) const;
      virtual btMatrix3x3 getTriangle(const int index) const;

    private:
    /**
     * Wrapper for Bullet DSK Mesh data 
     *  
     * @see QSharedData 
     *  
     * @author 2018-09-13 UA/OSIRIS-REx IPWG Team - Original Version
     */
      class BulletDskData : public QSharedData {
        public:
          typedef QScopedPointer<btTriangleIndexVertexArray>                  BtTriangleMesh;
          typedef QScopedPointer<double, QScopedPointerArrayDeleter<double> > BtVertexArray;
          typedef QScopedPointer<int, QScopedPointerArrayDeleter<int> >       BtIndexArray;

          BulletDskData() : QSharedData(), m_vertices(0), m_indexes(0),
                            m_btMesh(0), m_btIndex(0), m_btVertex(0) { 
              // allocate(0, 0);
          }
          ~BulletDskData() { }

          void allocate(const int nVertices, const int nIndexes) {
            m_btMesh.reset( new btTriangleIndexVertexArray() );
            m_btVertex.reset( new double[nVertices * 3] );
            m_btIndex.reset( new int[nIndexes * 3] );
          }

          int m_vertices;              //!< Number verticies in DSK
          int m_indexes;               //!< Number indexes in DSK
          BtTriangleMesh m_btMesh;     //!< Bullet mesh structure
          BtVertexArray  m_btVertex;   //!< Vertex array
          BtIndexArray   m_btIndex;    //!< Index array

        private:
          Q_DISABLE_COPY(BulletDskData);  // Undefined for this usage
      };

      QString                                    m_dskfile; /**! Name of DSK file */
      QExplicitlySharedDataPointer<BulletDskData> m_mesh; /**! Triangular mesh representation of
                                                              the target shape. The vertex ordering
                                                              is the same as in the DSK file,
                                                              except the DSK uses 1-based indexing
                                                              and this uses 0-based indexing. */

      // Custom DSK reader 
      void loadFromDsk(const QString &dskfile);

  };

} // namespace Isis

#endif

