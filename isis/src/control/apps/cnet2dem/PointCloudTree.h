#ifndef PointCloudTree_h
#define PointCloudTree_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cstdlib>
#include <vector>
// #include <pair>

#include <QSharedPointer>

#include <nanoflann.hpp>

#include "PointCloud.h"
#include "PointCloudSearchResult.h"

namespace Isis {

  /**
   * @brief Point cloud kd-tree class using the nanoflann kd-tree library
   *
   * This class renders a point cloud in a kd-tree for very fast/efficient point
   * queries. This particular implementation uses Nanoflann
   * (http://code.google.com/p/nanoflann/) kd-tree fast search query library.
   *
   * This class is specifically designed to support body-fixed 3D point vectors.
   * Its units are assumed to be kilometers, but is not a requirement.
   *
   * PointCloudTree utilizes the PointCloud and Point3d classes to provide fast,
   * efficient and flexible 3D vector proximity searches.  The implemenations of
   * these classes are optimized using inline methods to take advantage of the
   * template optimization of the Nanoflann library.
   *
   * Developers can decide at the time of the object instantiation to use 2D or 3D
   * Euclidean distances in its query with the nDim parameter in the constructor.
   * This flexibility sacrifices a little compiler optimization using template
   * parameters (see the -1 KcKdTree_t). The PointCloud classes support both 2D
   * and 3D lookups. 2D are common for mapping - 3D for apply restrictions in that
   * dimension for radius queries, particularly.
   *
   * It is important to note that the class will take ownership of the PointCloud
   * pointer required in the constructor.  This futher ensures that the PointCloud
   * will not be tampered with while the tree index is built from it.
   *
   * Query/search results are returned in a PCSearchResult object that can be used
   * to process the subset of points that satisfy the search parameters.  Along
   * with the results is a shared pointer to the PointCloud.  This provides access
   * to the Point3d points and their data variant for additional processing.
   *
   *  These classes are also designed to be thread-safe - maybe.
   *
   * @author 2014-02-17 Kris Becker
   *
   * @internal
   *   @history 2014-02-17 Kris Becker - Original Version
   */

  template <class T, class D> class PointCloudTree {

    private:
      // Declaration of the Nanoflann interface for our PointCloud.  Note the
      // DIM template parameter is defined by the type of distance compution
      // function type so the dimensionality of the distance can be
      // specified at compile time
      typedef nanoflann::KDTreeSingleIndexAdaptor<
                            nanoflann::L2_Simple_Adaptor<double, PointCloud<T,D> >,
                            PointCloud<T,D>, D::Dimension >  PcKdTree_t;

    public:
      /**
       * @brief Constructor of PointCloudTree for the kd-tree point representaion
       *
       * This constructor accepts a pointer to a prebuilt PointCloud.  Once provided,
       * this object takes over complete management/use of the PointCloud.  The nDim
       * parameter sets the dimensionality of the point.  Valid dimensions of the
       * kd-tree is 2 or 3.  The data provided in the PointCloud must support 2D or
       * 3D Euclidean distances.  The parameter, leafNodes, allows developers to
       * specify the number of leaves on each node.  See
       * https://code.google.com/p/nanoflann/#2._Any_help_choosing_the_KD-tree_parameters?
       * for additional details.
       *
       * @history 2014-03-03 Kris Becker Original Version
       *
       * @param pc        Pointer to PointCloud to build the kd-tree from.  This
       *                  object takes complete ownership of the PointCloud pointer.
       * @param leafNodes Maximum number of leaves stored at each kd-tree node
       */
      PointCloudTree(PointCloud<T,D> *pc, const int &leafNodes = 10) : m_pc(pc),
        m_kd_index(D::Dimension, *pc, nanoflann::KDTreeSingleIndexAdaptorParams(leafNodes)) {
        m_kd_index.buildIndex();
      }

      /** Destructor  */
      virtual ~PointCloudTree() { }

      /** Perform radius query for points in kilometer units */
      PointCloudSearchResult<T,D> radius_query(const T &point,
                                               const double &radius_sq) {
        std::vector<std::pair<size_t, double> > matches;

        int n = m_kd_index.radiusSearch(point.array(), radius_sq, matches,
                                        nanoflann::SearchParams());
        return (PointCloudSearchResult<T,D>(point, radius_sq, matches, m_pc, n));
      }

      /** Perform nearest set of points from its neighbors   */
      PointCloudSearchResult<T,D> neighbor_query(const T &point,
                                                  const int &neighbors) {
        QVector<size_t> indices(neighbors);
        QVector<double> distances(neighbors);
        m_kd_index.knnSearch(point.array(), neighbors, indices.data(),
                             distances.data());
        return (PointCloudSearchResult<T,D>(point, neighbors, indices, distances, m_pc));
      }

      /** Returns a reference to the point cloud */
      inline const PointCloud<T,D> &cloud() const { return ( *m_pc ); }

    private:
      QSharedPointer<PointCloud<T,D> >  m_pc;
      PcKdTree_t                        m_kd_index;

  };

};  // namespace Isis
#endif
