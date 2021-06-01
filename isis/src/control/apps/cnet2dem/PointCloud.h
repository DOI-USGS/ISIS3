#ifndef PointCloud_h
#define PointCloud_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QtGlobal>
#include <QVector>

namespace Isis {

  /**
   * Functor to compute 3-d Euclidean distances
   *
   * @author 2014-02-17 Kris Becker
   *
   * @internal
   *   @history 2014-02-17 Kris Becker - Original version.
   */
  template <class T> class Dist3d {
    public:
      enum { Dimension = 3 };
      Dist3d() { }
      ~Dist3d() { }

      inline int dimension() const {
        return ( Dimension );
      }

      inline double operator()(const T &datum1, const T &datum2) const {
        double dx = datum1.x() - datum2.x();
        double dy = datum1.y() - datum2.y();
        double dz = datum1.z() - datum2.z();
        return ( dx*dx + dy*dy + dz*dz );
      }

      inline double operator()(const double *datum1, const T &datum2) const {
        double dx = datum1[0] - datum2.x();
        double dy = datum1[1] - datum2.y();
        double dz = datum1[2] - datum2.z();
        return ( dx*dx + dy*dy + dz*dz );
      }
  };


  /**
   * Functor to compute 2-d Euclidean distances
   *
   * @author 2014-02-17 Kris Becker
   *
   * @internal
   *   @history 2014-02-17 Kris Becker - Original version.
   */
  template <class T> class Dist2d {
    public:
      enum { Dimension = 2 };
      Dist2d() { }
      ~Dist2d() { }

      inline int dimension() const {
        return ( Dimension );
      }

      inline double operator()(const T &datum1, const T &datum2) const {
        double dx = datum1.x() - datum2.x();
        double dy = datum1.y() - datum2.y();
        return ( dx*dx + dy*dy );
      }

      inline double operator()(const double *datum1, const T &datum2) const {
        double dx = datum1[0] - datum2.x();
        double dy = datum1[1] - datum2.y();
        return ( dx*dx + dy*dy );
      }
  };


  /**
   * Functor to compute 1-d Manhattan distances
   *
   * @author 2014-02-17 Kris Becker
   *
   * @internal
   *   @history 2014-02-17 Kris Becker - Original version.
   */
  template <class T> class Dist1d {
    public:
      enum { Dimension = 1 };
      Dist1d() { }
      ~Dist1d() { }

      inline int dimension() const {
        return ( Dimension );
      }


      inline double operator()(const T &datum1, const T &datum2) const {
        double dx = datum1.x() - datum2.x();
        return ( dx*dx );
      }

      inline double operator()(const double *datum1, const T &datum2) const {
        double dx = datum1[0] - datum2.x();
        return ( dx*dx );
      }
  };


  /**
   * @brief Point cloud adapter class for nanoflann kd-tree interface
   *
   * This class provides the point cloud class for Point3d clouds that interface
   * with the nanoflann (http://code.google.com/p/nanoflann/) kd-tree fast search
   * query library. The reference for this approach can be found in an example
   * found at
   * http://nanoflann.googlecode.com/svn/trunk/examples/pointcloud_kdd_radius.cpp.
   *
   * This class is designed to accept a reference to a container of 3-d points.
   * In addition, this class accesses individual points from PointCloud using a
   * vector component operator so as to standardize and complete this interface:
   *
   * @code
   *   double x() const;  // X component of point
   *   double y() const;  // Y component of point
   *   double z() const;  // Z component of point
   *   double w() const;  // Optional weight of point (default should be 1.0)
   * @endcode
   *
   * The point container is required to not change its content for the duration
   * of use of the nanoflann kd-tree built from the points. Because of this,
   * there is no clear() method to discard existing points.
   *
   * This class supports 2D and 3D Euclidean distant calculations. This option is
   * specified in the PointCloudTree class constructor when building the kd-tree
   * index.
   *
   * The routines kdtree_get_point_count(), kdtree_distance(), kdtree_get_pt()
   * and kdtree_get_bbox() (default implementation) satisfy the needs of the
   * Nanoflann kd-tree template library.
   *
   * This class is not a template, but all its methods are inlined for efficiency
   * reasons. This implemetation approach allows us to take best advantage of the
   * optimization that the Nanoflann library offers.
   *
   * The Point3d class is also optimized in the same fashion and offers a
   * flexible point base class to complete the implementation.
   *
   * This point cloud class is designed with the body-fixed coordinate system in
   * mind. Therfore, the units of the point vectors is assumed to be kilometers
   * but this is not required - as long as Euclidean distances apply to the point
   * dataset, any 3D vector representation could utilize this class.
   *
   * @author 2014-02-17 Kris Becker
   *
   * @internal
   *   @history 2014-02-17 Kris Becker - Original version.
   */
  template <class T, class Distance = Dist3d<T> > class PointCloud {
    public:
      PointCloud() : m_distance(), m_points()  { }
      PointCloud(const Distance &functor) : m_distance(functor), m_points()  { }
      PointCloud(const int &npoints, const Distance &functor = Distance()) :
                 m_distance(functor), m_points() {
        m_points.reserve(npoints);
      }

      PointCloud(const QVector<T> &points) : m_points(points) { }
      virtual ~PointCloud() { }


      /** Standard size method */
      inline int size() const {
        return ( m_points.size() );
      }

      Distance &getDistance() {
        return ( m_distance );
      }

      /** Add a new point to the list */
      inline void addPoint(const T &point) {
        m_points.push_back(point);
      }

      /** Return a reference to the point at index idx */
      inline const T &point(const size_t idx) const {
        Q_ASSERT( idx >= 0 );
        Q_ASSERT( idx < (size_t) size() );
        return (m_points[idx]);
      }

      inline double distance( const T &first, const T &second ) const {
        return ( m_distance(first, second) );
      }

      /** Return number of points in cloud */
      inline size_t kdtree_get_point_count() const {
        return ( m_points.size() );
      }

    /**
     * @brief Return Euclidean distance from a source point to the indexed point
     *
     * This method returns the Euclidean (L2) distance from a dataset point and a
     * source point (p1).  We only use X and Y components to compute this
     * distance since our objective is to identify points in 2-d space.
     *
     * @author 2014-02-27 Kris Becker
     *
     * @param p1      3-vector of the source point
     * @param idx_p2  Index into point data set contained herein
     *
     * @return double Returns the squared distance - not the square root!
     */
      inline double kdtree_distance(const double *p1, const size_t idx_p2,
                                    size_t p_size) const {
        Q_ASSERT( idx_p2 >= 0 );
        Q_ASSERT( idx_p2 < (size_t) size() );
        return ( m_distance(p1, point(idx_p2)) );
      }

    /**
     * @brief Returns a value for a single dimemsion of a vector
     *
     * This method provides a simple interface to each vector element in the point
     * cloud.
     *
     * @history 2014-03-03 Kris Becker
     *
     * @param idx Index of the point to get element from
     * @param dim The index (0 - 2)of the ith element of the vector
     *
     * @return double Value at the specfied vector point index
     */
      inline double kdtree_get_pt(const size_t idx, int dim) const {
        if ( dim == 0 )  return ( point(idx).x() );
        if ( dim == 1 )  return ( point(idx).y() );
        return ( point(idx).z() );
      }

      /** Let nanoflann range algorithm compute the bounding box */
      template <class BBOX> bool kdtree_get_bbox(BBOX &bb) const {
        return (false);
      }

    private:
      Distance   m_distance; ///!< Instantiation of distance functor
      QVector<T> m_points;   ///!< points in the point cloud

  };

};  // namespace Isis
#endif
