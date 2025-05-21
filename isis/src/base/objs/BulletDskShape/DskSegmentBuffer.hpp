#ifndef DskSegmentBuffer_h
#define DskSegmentBuffer_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <memory>
#include <string>
#include <iostream>
#include <iomanip>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "IsisBullet.h"
#include "IException.h"

namespace Isis {

/**
 * DSK Segment data container for mesh access
 * 
 * This class maintains a DSK segment which represents and shape of a target
 * body. Each segment triangular mesh veticies and indexes are extracted into
 * this container. Methods provide support for accessing data properly and
 * allocation and manipulating the data buffers.
 * 
 * One other important method is to create a Bullet mesh array of the DSK
 * contents. It will ensure all the plates do not exceed any Bullet limits.
 * It is up to the caller to ensure validaty of this configuration.
 *
 * @author 2025-05-09 Kris Becker
 * @internal
 *   @history 2025-05-09  Kris J. Becker, University of Arizona
 */
  class DskSegmentBuffer {
    public:
      DskSegmentBuffer( ) : m_segment_no(-1), m_dsk_descr{},
                            m_vertex_count(0), m_index_count(0),
                            m_vertex_data(), m_index_data() { }                  
      DskSegmentBuffer(const int segmentno, const SpiceDLADescr &dla ) : 
                        m_segment_no( segmentno ), m_dsk_descr( dla ),
                        m_vertex_count(0), m_index_count(0),
                        m_vertex_data(), m_index_data() { }
      DskSegmentBuffer(const int segmentno, const SpiceDLADescr &dla,
                       const int nplates, const int nvertices ) : 
                        m_segment_no( segmentno ), m_dsk_descr( dla ),
                        m_vertex_count(0), m_index_count(0),
                        m_vertex_data(), m_index_data() { 
        this->allocate( nplates, nvertices );
      }  

      virtual ~DskSegmentBuffer() { }

      inline bool isValid() const {
        return ( -1 != this->segment_number() );
      }

      inline int segment_number() const {
        return ( m_segment_no );
      }      

      inline size_t vertex_count() const {
        return ( m_vertex_count );
      }

      inline size_t plate_count() const {
        return ( m_index_count );
      }          

      inline const SpiceDLADescr &dla() const {
        return ( m_dsk_descr );
      }

      inline unsigned char *vector_ptr( const size_t vth = 0 ) const {
        return ( m_vertex_data.get() + ( vth * 3 * sizeof(double) ) );
      }

      inline double *vector( const size_t vth = 0 ) const {
        return ( reinterpret_cast<double *> ( this->vector_ptr( vth ) ) );
      }        

      inline unsigned char *index_ptr( const size_t ith = 0 ) const {
        return ( m_index_data.get() + ( ith * 3 * sizeof(int) ) );
      }

      inline int *index( const size_t vth = 0 ) const {
        return ( reinterpret_cast<int *> ( this->index_ptr( vth ) ) );
      }

      inline void add_index_offset( const int offset_t ) {
        for ( size_t ith = 0 ; ith < this->plate_count() ; ith++ ) {
          int *index = this->index( ith );
          index[0] += offset_t;
          index[1] += offset_t;
          index[2] += offset_t;
        }
      }

      /** Allocate buffers for the DSK segment */
      inline void allocate( const int nplates, const int nvertices ) {
        try {
          m_vertex_data.reset( new unsigned char[ nvertices * 3 * sizeof(double) ] );
          m_index_data.reset( new unsigned char[ nplates * 3 * sizeof(int) ] );
        }
        catch (... ) {
          std::string mess = "** DSK Data Allocation Error *** segment " + std::to_string( m_segment_no ) + ", plates ("
                             + std::to_string(nplates) + ", vertices (" + std::to_string( nvertices) + ")";
          throw IException(IException::User, mess, _FILEINFO_);      
        }

        m_vertex_count = nvertices;
        m_index_count  = nplates;
      }

      /** Add the buffer contents to the mesh */
      inline int addtomesh( btTriangleIndexVertexArray &btmesh ) const {
        const size_t max_bt_triangles = bt_MaxTriangles();

#if defined(DSK_DEBUG)
        std::cout << "\n*** Creating Bullet Mesh ***" << std::endl;
        std::cout << "Segment:   " << this->segment_number() << std::endl;
        std::cout << "Vectors:   " << this->vertex_count() << std::endl;
        std::cout << "Indexes:   " << this->plate_count() << std::endl;
#endif

        size_t n_facets_mapped = 0;
        int    n_parts = 0;
        while ( n_facets_mapped < this->plate_count() ) {
          size_t n_remaining = this->plate_count() - n_facets_mapped;
          size_t n_mesh_facets = std::min( n_remaining, max_bt_triangles );

          btIndexedMesh v_mesh;
          v_mesh.m_vertexType = PHY_DOUBLE;

          // Set and allocate data for triangle indexes
          v_mesh.m_numTriangles        = n_mesh_facets;
          v_mesh.m_triangleIndexBase   = this->index_ptr( n_facets_mapped );
          v_mesh.m_triangleIndexStride = (sizeof(int) * 3);
    
          // Set and allocate vertex data
          v_mesh.m_numVertices  = this->vertex_count();
          v_mesh.m_vertexBase   = this->vector_ptr( 0 );
          v_mesh.m_vertexStride = (sizeof(double) * 3); 
          
          // Add the mesh the the index array
          btmesh.addIndexedMesh( v_mesh, PHY_INTEGER );

#if defined(DSK_DEBUG)
          std::cout <<   "PartNo:    " << n_parts << std::endl;
          std::cout <<   "Plates:    " << n_mesh_facets << std::endl;
#endif              

          // Update status
          n_parts++;
          n_facets_mapped += n_mesh_facets;
        }
        
        return ( n_parts );
      }

    private:
      // Contents for the DSK segment with buffer support
      int                              m_segment_no;
      SpiceDLADescr                    m_dsk_descr;
      SpiceInt                         m_vertex_count;
      SpiceInt                         m_index_count;
      std::shared_ptr<unsigned char[]> m_vertex_data;
      std::shared_ptr<unsigned char[]> m_index_data;      
  };

} // namespace Isis

#endif
