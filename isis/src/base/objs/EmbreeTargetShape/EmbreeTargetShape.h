#ifndef EmbreeTargetShape_h
#define EmbreeTargetShape_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

// Embree includes
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>

// PointCloudLibrary includes
#include <pcl/io/auto_io.h>
#include <pcl/point_types.h>
#include <pcl/PolygonMesh.h>

//  WARNING
//  The following undefes macros set in pcl. They conflict with ISIS's macros.
//  If pcl tries to use these macros you will get compilation errors.
//    - JAM
#undef DEG2RAD
#undef RAD2DEG

#include "FileName.h"
#include "LinearAlgebra.h"

namespace Isis {


  class Pvl;

  /**
   * Struct for capturing multiple intersections when using
   * embree::rtcintersectscene.
   * 
   * @author 2017-05-11 Jesse Mapel
   * @internal 
   *   @history 2017-05-11 Jesse Mapel - Original Version
   */
  struct RTCMultiHitRay : RTCRay {
    RTCMultiHitRay();
    RTCMultiHitRay(const std::vector<double> &origin,
                   const std::vector<double> &direction);
    RTCMultiHitRay(LinearAlgebra::Vector origin, 
                   LinearAlgebra::Vector direction);

// These are the inheritted members from RTCRay
//   float    org;    //!< Ray origin
//   float    dir;    //!< Ray direction
//   float    tnear;  //!< Start of ray segment
//   float    tfar;   //!< End of ray segment
//   float    time;   //!< Time of this ray for motion blur.
//   unsigned mask;   //!< used to mask out objects during traversal
//   float    Ng;     //!< Geometric normal.
//   float    u;      //!< Barycentric u coordinate of hit
//   float    v;      //!< Barycentric v coordinate of hit
//   unsigned geomID; //!< geometry ID
//   unsigned primID; //!< primitive ID
//   unsigned instID; //!< instance ID

    // ray extensions
    // we remember up to 16 hits to ignore duplicate hits
    unsigned hitGeomIDs[16]; //!< IDs of the geometries (bodies) hit
    unsigned hitPrimIDs[16]; //!< IDs of the primitives (trinagles) hit
    float    hitUs[16];      //!< Barycentric u coordinate of the hits
    float    hitVs[16];      //!< Barycentric v coordinate of the hits
    int      lastHit;        //!< Index of the last hit in the hit containers
  };


  /**
   * Struct for capturing occluded plates when using
   * embree::rtcintersectscene.
   * 
   * @author 2017-05-15 Kristin Berry
   * @internal 
   *   @history 2017-05-15 Kristin Berry - Original Version
   */
  struct RTCOcclusionRay : RTCRay {
    RTCOcclusionRay();
    RTCOcclusionRay(const std::vector<double> &origin,
                   const std::vector<double> &direction);
    RTCOcclusionRay(LinearAlgebra::Vector origin, 
                   LinearAlgebra::Vector direction);

// These are the inheritted members from RTCRay
//   float    org;    //!< Ray origin
//   float    dir;    //!< Ray direction
//   float    tnear;  //!< Start of ray segment
//   float    tfar;   //!< End of ray segment
//   float    time;   //!< Time of this ray for motion blur.
//   unsigned mask;   //!< used to mask out objects during traversal
//   float    Ng;     //!< Geometric normal.
//   float    u;      //!< Barycentric u coordinate of hit
//   float    v;      //!< Barycentric v coordinate of hit
//   unsigned geomID; //!< geometry ID
//   unsigned primID; //!< primitive ID
//   unsigned instID; //!< instance ID

    // ray extensions
    int lastHit;            //!< Index of the last hit in the hit containers
    unsigned ignorePrimID; //!< IDs of the primitives (trinagles) which should be ignored. 
  };


  /**
   * Container that holds the body fixed intersection point and unit surface
   * normal for a hit.
   * 
   * @author 2017-05-11 Jesse Mapel
   * @internal 
   *   @history 2017-05-11 Jesse Mapel - Original Version
   */
  struct RayHitInformation {
    RayHitInformation();
    RayHitInformation(LinearAlgebra::Vector &location, LinearAlgebra::Vector &normal, int primID);

    LinearAlgebra::Vector intersection;  //!< The (x, y, z) intersection location
    LinearAlgebra::Vector surfaceNormal; //!< The unit surface normal vector at the intersection
    int primID; //!< The primitive ID of the hit. 
  };


/**
 * @brief Embree Target Shape for planetary bodies 
 *  
 * This class holds the Embree representation of a target body. All vectors
 * are expected to be in the body-fixed reference frame for the target and all
 * positions are expected to be in kilometers.
 * 
 * @author 2017-05-11 Jeannie Backer & Jesse Mapel
 * @internal 
 *   @history 2017-05-11 Jeannie Backer & Jesse Mapel - Original Version
 */
  class EmbreeTargetShape {
    public:

      EmbreeTargetShape();
      EmbreeTargetShape(pcl::PolygonMesh::Ptr mesh, const QString &name = "");
      EmbreeTargetShape(const QString &dem, const Pvl *conf = 0);
      virtual ~EmbreeTargetShape();

      QString name() const;
      bool isValid() const;

      int numberOfPolygons() const;
      int numberOfVertices() const;
      RTCBounds sceneBounds() const;
      double maximumSceneDistance() const;

      void intersectRay(RTCMultiHitRay &ray);
      bool isOccluded(RTCOcclusionRay &ray);

      RayHitInformation getHitInformation(RTCMultiHitRay &ray, int hitIndex);

      static void multiHitFilter(void* userDataPtr, RTCMultiHitRay& ray);
      static void occlusionFilter(void* userDataPtr, RTCOcclusionRay& ray);

    protected:
      pcl::PolygonMesh::Ptr readDSK(FileName file);
      pcl::PolygonMesh::Ptr readPC(FileName file);
      void initMesh(pcl::PolygonMesh::Ptr mesh);
      void addVertices(int geomID);
      void addIndices(int geomID);

    private:
      /**
       * Container for a vertex.
       * 
       * @author 2017-03-27 Ian Humphrey
       * @internal 
       *   @history 2017-03-27 Ian Humphrey - Original Version
       */
      struct Vertex {
        float x; //!< Vertex x position
        float y; //!< Vertex y position
        float z; //!< Vertex z position
        float a; //!< Extra float for aligning memory
      };

      /**
       * Container for a tin, or triangular polygon. The vertices are expected
       * to be ordered counter-clockwise about the surface normal.
       * 
       * @author 2017-03-27 Ian Humphrey
       * @internal 
       *   @history 2017-03-27 Ian Humphrey - Original Version
       */
      struct Triangle {
        int v0; //!< The index of the first vertex in the tin.
        int v1; //!< The index of the second vertex in the tin.
        int v2; //!< The index of the third vertex in the tin.
      };

      QString                        m_name;   /**!< The name of the target. */
      pcl::PolygonMesh::Ptr          m_mesh;   /**!< A boost shared pointer to
                                                     the polygon mesh representation
                                                     of the target. */
      pcl::PointCloud<pcl::PointXYZ> m_cloud;  /**!< The point cloud representation
                                                     of the target. This is also
                                                     stored in the polygon mesh,
                                                     but cannot be accessed, so
                                                     it is duplicated here. */
      RTCDevice                      m_device; /**!< The Embree device for rendering
                                                     the scene. */
      RTCScene                       m_scene;  /**!< The Embree scene that holds
                                                     Embree's representation of
                                                     the target body and the aabb
                                                     tree used to accelerate ray
                                                     tracing. */

  };

} // namespace Isis

#endif

