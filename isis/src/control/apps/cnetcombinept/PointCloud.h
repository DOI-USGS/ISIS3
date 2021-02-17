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
 *  Functor to compute 2-d Euclidean distances
 *
 * @author 2014-02-17 Kris Becker
 *
 * @internal
 *   @history 2014-02-17 Kris Becker - Original Version
 *   @history 2016-12-06 Jesse Mapel - Updated documentation. References #4558.
 */
template <class T> class Dist2d {
public:
  enum { Dimension = 2 //!< The dimension of the distance metric.
  };

  /**
   * Constructs a Dist2d functor.
   */
  Dist2d() { }


  /**
   * Destroys a Dist2d functor.
   */
  ~Dist2d() { }


  /**
   * Returns the dimension of the distance metric.
   *
   * @return @b int Always 2.
   */
  inline int dimension() const {
    return ( Dimension );
  }


  /**
   * Computes the two dimensional distance between two points.
   *
   * @param datum1 The first point
   * @param datum2 The second point
   *
   * @return @b double The distance between the two points.
   */
  inline double operator()(const T &datum1, const T &datum2) const {
    double dx = datum1.x() - datum2.x();
    double dy = datum1.y() - datum2.y();
    return ( dx*dx + dy*dy );
  }


  /**
   * Computes the two dimensional distance between two points.
   *
   * @param datum1 The first point as a double array
   * @param datum2 The second point
   *
   * @return @b double The distance between the two points.
   */
  inline double operator()(const double *datum1, const T &datum2) const {
    double dx = datum1[0] - datum2.x();
    double dy = datum1[1] - datum2.y();
    return ( dx*dx + dy*dy );
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
 *  The point container is required to not change its content for the duration
 *  of use of the nanoflann kd-tree built from the points. Because of this,
 *  there is no clear() method to discard existing points.
 *
 *  This class supports 2D and 3D Euclidean distant calculations. This option is
 *  specified in the PointCloudTree class constructor when building the kd-tree
 *  index.
 *
 *  The routines kdtree_get_point_count(), kdtree_distance(), kdtree_get_pt()
 *  and kdtree_get_bbox() (default implementation) satisfy the needs of the
 *  Nanoflann kd-tree template library.
 *
 *  This class is not a template, but all its methods are inlined for efficiency
 *  reasons. This implemetation approach allows us to take best advantage of the
 *  optimization that the Nanoflann library offers.
 *
 *  The Point3d class is also optimized in the same fashion and offers a
 *  flexible point base class to complete the implementation.
 *
 *  This point cloud class is designed with the body-fixed coordinate system in
 *  mind. Therfore, the units of the point vectors is assumed to be kilometers
 *  but this is not required - as long as Euclidean distances apply to the point
 *  dataset, any 3D vector representation could utilize this class.
 *
 * @author 2014-02-17 Kris Becker
 *
 * @internal
 *   @history 2014-02-17 Kris Becker - Original Version
 */
template <class T > class PointCloud {
  public:

    /**
     * Constructs a default PointCloud.
     */
    PointCloud() : m_id("PointCloud"), m_points(), m_distance() { }


    /**
     * Constructs a PointCloud with a given ID.
     *
     * @param id The ID of the PointCloud.
     */
    explicit PointCloud(const QString &id) :  m_id(id), m_points(), m_distance()  { }


    /**
     * Constructs a PointCloud with a given ID and
     * space for a given number of points reserved.
     *
     * @param npoints The number of points to reserve space for.
     * @param id The ID of the PointCloud.  Defaults to "PointCloud".
     */
    PointCloud(const int &npoints, const QString &id = "PointCloud") :
               m_id(id), m_points(), m_distance() {
      m_points.reserve(npoints);
    }


    /**
     * Constructs a PointCloud with a given ID and set of points.
     *
     * @param points The set of points to add to the PointCloud.
     * @param id The ID of the PointCloud.  Defaults to "PointCloud".
     */
    PointCloud(const QVector<T> &points, const QString &id = "PointCloud") :
               m_id(id), m_points(points), m_distance() { }


    /**
     * Constructs a PointCloud with a given ID and set measures.
     * The measures will be used to construct the internal set of points.
     *
     * @param points The set of measures to add to the PointCloud.
     * @param id The ID of the PointCloud.  Defaults to "PointCloud".
     */
    PointCloud(const QList<ControlMeasure *> &points,
               const QString &id = "PointCloud") :
               m_id(id), m_points(), m_distance() {
      for (int i = 0 ; i < points.size() ; i++ ) {
        m_points.push_back( T(points[i]) );
      }
    }


    /**
     * Desctroys the PointCloud.
     */
    virtual ~PointCloud() { }


    /**
     * Returns the size of the PointCloud.
     *
     * @return @b int The number of points in the PointCloud.
     */
    inline int size() const {
      return ( m_points.size() );
    }


    /**
     * Returns the PointCloud's ID.
     *
     * @return @b QString The PointCloud's ID.
     */
    inline QString identifier() const {
      return (m_id);
    }


    /**
     * Adds a new point to the list
     *
     * @param point The point to add.
     */
    inline void addPoint(const T &point) {
      m_points.push_back(point);
    }


    /**
     * Return a reference to the point at a given index.
     *
     * @param idx The index of the point to return.
     *
     * @return @b T& A constant reference to the point.
     */
    inline const T &point(const size_t idx) const {
      Q_ASSERT( idx >= 0 );
      Q_ASSERT( idx < (size_t) size() );
      return (m_points[idx]);
    }


    /**
     * Returns the Euclidean distance between two points.
     *
     * @param first The first point.
     * @param second The second point.
     *
     * @return @b double The squared Euclidean distance between first and second.
     */
    inline double distance( const T &first, const T &second ) const {
      return ( m_distance(first, second) );
    }


    /**
     * Returns the number of points in the PointCloud.
     *
     * @return @b size_t The number of points in the PointCloud.
     */
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
   * @return @b double Returns the squared distance - not the square root!
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
   * @return @b double Value at the specfied vector point index
   */
    inline double kdtree_get_pt(const size_t idx, int dim) const {
      if ( dim == 0 )  return ( point(idx).x() );
      if ( dim == 1 )  return ( point(idx).y() );
      return ( point(idx).z() );
    }


    /**
     *  Let nanoflann range algorithm compute the bounding box
     *
     * @author 2014-02-17 Kris Becker
     *
     * @internal
     *   @history 2014-02-17 Kris Becker - Original Version
     */
    template <class BBOX> bool kdtree_get_bbox(BBOX &bb) const {
      return (false);
    }

  private:

    QString    m_id;       ////! Identifier.
    QVector<T> m_points;   ///!< Points in the point cloud.
    Dist2d<T>  m_distance; ///!< Instantiation of distance functor.

};

};  // namespace Isis
#endif
