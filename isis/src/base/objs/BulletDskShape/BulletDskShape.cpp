/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BulletDskShape.h"

#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>

#include "NaifDskApi.h"

#include <QMutexLocker>
#include <QStringList>
#include <QTime>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Pvl.h"
#include "NaifDskPlateModel.h"
#include "NaifStatus.h"


using namespace std;

namespace Isis {

  /**
   * Default empty constructor.
   */
  BulletDskShape::BulletDskShape() :  m_mesh() { }


  /**
   * Construct a BulletDskShape from a DSK file.
   *
   * @param dskfile The DSK file to load into a Bullet target shape.
   */
  BulletDskShape::BulletDskShape(const QString &dskfile) : m_mesh()  {
    loadFromDsk(dskfile);
    setMaximumDistance();
  }


  /**
   * Destructor
   */
  BulletDskShape::~BulletDskShape() { }

  /**
   * Return the number of triangles in the shape
   *
   * @return @b int The number of triangles. If nothing has been loaded, then 0 is returned.
   */
  int BulletDskShape::getNumTriangles() const {
    size_t num_triangles = 0;

    if (m_mesh) {
      for(int i = 0; i < m_mesh->getIndexedMeshArray().size(); i++) {
        num_triangles += m_mesh->getIndexedMeshArray()[i].m_numTriangles;
      }
    }

    return num_triangles;
  }


  /**
   * Return the number of verticies in the shape
   *
   * @return @b int The number of verticies. If nothing has been loaded, then 0 is returned.
   */
  int BulletDskShape::getNumVertices() const {
    size_t num_vertices = 0;

    if (m_mesh) {
      for(int i = 0; i < m_mesh->getIndexedMeshArray().size(); i++) {
        num_vertices += m_mesh->getIndexedMeshArray()[i].m_numVertices;
      }
    }

    return num_vertices;
  }


  /**
  * @brief Return normal for a given triangle index
  *
  * This method is particularly useful to return the normal of a triangle plate
  * in a mesh-based target body.
  *
  * @author 2017-03-28 Kris Becker
  *
  * @param indexId The index of the triangle in the mesh.
  *
  * @return @b btVector3 The local normal for the triangle.
  */
  btVector3 BulletDskShape::getNormal(const int indexId, const int segment) const {
    btMatrix3x3 triangle = getTriangle(indexId, segment);
    btVector3 edge1 = triangle.getRow(1) - triangle.getRow(0);
    btVector3 edge2 = triangle.getRow(2) - triangle.getRow(0);
    return ( edge1.cross( edge2 ) );
  }


  /**
   * Get the vertices of a triangle in the mesh.
   *
   * @param index The index of the triangle in the mesh.
   *
   * @return @b btMatrix3x3 Matrix with each row containing the coordinate of a
   *                        vertex. The vertices are ordered counter-clockwise
   *                        around the surface normal of the triangle.
   */
  btMatrix3x3 BulletDskShape::getTriangle(const int index, const int segment) const {
    btAssert ( index >= 0 );
    btAssert ( index < getIndexedMeshArray()[segment].m_numTriangles );

    btAssert ( segment >= 0 );
    btAssert ( segment < getIndexedMeshArray().size());

     // Set up pointers to triangle indexes
    const btIndexedMesh &v_mesh = m_mesh->getIndexedMeshArray()[segment];

    const int *t_index = static_cast<int32_t *> ((void *) v_mesh.m_triangleIndexBase);
    int p_index = 3 * index;
    int vndx0 = t_index[p_index];
    int vndx1 = t_index[p_index+1];
    int vndx2 = t_index[p_index+2];

    const btScalar *t_vertex = static_cast<const btScalar *> ((void *) v_mesh.m_vertexBase);

    btMatrix3x3 triangle(t_vertex[vndx0+0], t_vertex[vndx0+1], t_vertex[vndx0+2],
                         t_vertex[vndx1+0], t_vertex[vndx1+1], t_vertex[vndx1+2],
                         t_vertex[vndx2+0], t_vertex[vndx2+1], t_vertex[vndx2+2]);
    return ( triangle );
  }


/**
 * @brief Load the contents of a NAIF DSK and create a Bullet triangle mesh
 *
 * Do realtime validation of Bullet mesh limits. Ensure DSK segment shapes
 * fit properly in mesh Bullet parts to enable quantized optimzation.
 * 
 * See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/dsk.html#Appendix%20B%20---%20DSK%20Subsystem%20Limits
 *
 * Bullet limits in the repo are currently set at 4 parts and 134,217,728
 * triangles. However, that version has not been released since the Aug 2022
 * commit. Hence, we must ensure that all DSK segment shapes fit into the
 * Bullet mesh mapping scheme which currently has 1024 parts which supports
 * no more than 2,097,152 triangles. At any rate, loading DSKs must consider
 * these limits when mapping to bullet. So quantized optimization for DSK
 * support is limited to at most 1024 segments where each segment has no more
 * than 2,097,152 facets.
 *
 * See https://github.com/bulletphysics/bullet3/blob/master/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.h
 *
 * This configuration can change in future Bullet releases and may impact use
 * of quantized optimizations for DSKs. Larger DSKs can still (apparently) be
 * used in Bullet if these limits are exceeded but quantized optimzations of
 * the Bullet mesh is disabled for these types of DSKs.
 * 
 * @author 2017-03-28 Kris Becker
 * @history 2025-05-11 Kris J Becker - Updated implementation to provide
 *                       generic support for Bullet quantized optimization
 *                       limits. Fixes #5772.
 *
 * @param dskfile The DSK file to load.
 */
  void BulletDskShape::loadFromDsk(const QString &dskfile) {

    /** NAIF DSK parameter setup   */
    SpiceInt      handle;   //!< The DAS file handle of the DSK file.
    SpiceBoolean  found;
    SpiceDLADescr segment;

    // Sanity check
    FileName dskFile(dskfile);
    if ( !dskFile.fileExists() ) {
      QString mess = "NAIF DSK file [" + dskfile + "] does not exist.";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Open the NAIF Digital Shape Kernel (DSK)
    dasopr_c( dskFile.expanded().toLatin1().data(), &handle );
    NaifStatus::CheckErrors();

    // Search to the first DLA segment
    dlabfs_c( handle, &segment, &found );
    NaifStatus::CheckErrors();
    if ( !found ) {
      QString mess = "No segments found in DSK file " + dskfile ;
      throw IException(IException::User, mess, _FILEINFO_);
    }

  #if defined(DSK_DEBUG)
    std::cout << "Maximum Bullet Parts:   " << bt_MaxBodyParts() << std::endl;
    std::cout << "Maximum Triangles/Part: " << bt_MaxTriangles() << std::endl;
  #endif

    // Now allocate a new indexed mesh to contain all the DSK data
    // Clear any existing buffers
    m_mesh.reset( new btTriangleIndexVertexArray() );
    m_buffers.clear();

    int n_parts = 0;
    int n_segments = 0;

    while( found ) {
      
      // Validate last segment found before searching for the next segment
      SpiceInt s_plates;
      SpiceInt s_vertices;
      dskz02_c( handle, &segment, &s_vertices, &s_plates);
      NaifStatus::CheckErrors();

#if defined(DSK_DEBUG)
      std::cout << "\nSegment:   " << n_segments << std::endl;
      std::cout << "#Vertices: " << s_vertices << std::endl;
      std::cout << "#Plates:   " << s_plates << std::endl;
#endif

      // Initialize the DSK data buffer container
      DskSegmentBuffer dsk_buffer(n_segments, segment, s_plates, s_vertices );

      SpiceInt n;
      (void) dskv02_c(handle, &segment, 1, s_vertices, &n,
                      ( SpiceDouble(*)[3] ) ( dsk_buffer.vector_ptr(0)));
      NaifStatus::CheckErrors();
 
      // Read the indexes from the DSK
      (void) dskp02_c(handle, &segment, 1, s_plates, &n,
                     ( SpiceInt(*)[3] ) (dsk_buffer.index_ptr(0)));
      NaifStatus::CheckErrors();
       
      // Subtract one from the index to make it 0-based and add to Bullet mesh
      dsk_buffer.add_index_offset( -1 );
      m_buffers.push_back( dsk_buffer );
      n_parts += dsk_buffer.addtomesh( *m_mesh );
      n_segments++;

      // Search for the next segment and retain for next loop (above)
      dlafns_c(handle,  &dsk_buffer.dla(), &segment, &found);
      NaifStatus::CheckErrors();
    }
    
#if defined(DSK_DEBUG)
    std::cout << "\n#Segments: " << m_buffers.size() << std::endl;
    std::cout <<   "#Parts:    " << n_parts << std::endl;
#endif    

    // Close DSK
    dascls_c(handle);

    // Set up the triange mesh and target object
    bool useQuantizedAabbCompression = true;
    if ( n_parts > bt_MaxBodyParts() ) {
      std::cout << "*** WARNING ***  BulletDskShape total mesh parts (" << n_parts 
                << ") exceeds Bullet max (" << bt_MaxBodyParts() << ") - quantized AABB compression disabled"
                << std::endl;

      // Cannot use quantized compresssio
      useQuantizedAabbCompression = false;
    }

    // Note the btCollisionObject is managed in this class, see pointer allocations
    btBvhTriangleMeshShape *v_triShape = new btBvhTriangleMeshShape(m_mesh.get(),
                                                                    useQuantizedAabbCompression);
    v_triShape->setUserPointer(this);
    btCollisionObject *vbody = new btCollisionObject();
    vbody->setCollisionShape(v_triShape);
    setTargetBody(vbody);

    return;

  }

}  // namespace Isis
