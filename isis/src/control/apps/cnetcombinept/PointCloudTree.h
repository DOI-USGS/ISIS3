#ifndef PointCloudTree_h
#define PointCloudTree_h
/**
 * @file
 * $Revision: 1.0 $ 
 * $Date: 2014/02/27 18:49:25 $ 
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <cstdlib>
#include <vector>
// #include <pair>

#include <QSharedPointer>

#include <nanoflann.hpp>

#include "PointCloud.h"

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
 *   @history 2016-12-06 Jesse Mapel - Updated documentation. References #4558.
 */

template <class T> class PointCloudTree {

 private:
   // Declaration of the Nanoflann interface for our PointCloud.  Note the
   // DIM template parameter is defined by the type of distance compution 
   // function type so the dimensionality of the distance can be 
   // specified at compile time
  typedef nanoflann::KDTreeSingleIndexAdaptor<
                        nanoflann::L2_Simple_Adaptor<double, PointCloud<T> >, 
                        PointCloud<T>, 2 >  PcKdTree_t;

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
   * @param leafNodes Maximum number of leaves stored at each kd-tree node.
   *                  Defaults to 10 leaves.
   */
    PointCloudTree(PointCloud<T> *pc, const int &leafNodes = 10) : m_pc(pc),
       m_kd_index(2, *pc, nanoflann::KDTreeSingleIndexAdaptorParams(leafNodes)) {
       m_kd_index.buildIndex();
    } 


    /**
     * Destroys a PointCloudTree.
     */
    virtual ~PointCloudTree() { }


    /** 
     * Performs a radius query for points in kilometer units.
     * 
     * @param point The point to search around.
     * @param radius_sq The square of the radius to search around point.
     * 
     * @return @b QList<T> A list of all the points within the radius.
     */
    QList<T> radius_query(const T &point, const double &radius_sq) { 
      std::vector<std::pair<size_t, double> > matches;

      (void) m_kd_index.radiusSearch(point.array(), radius_sq, matches, 
                                     nanoflann::SearchParams());

      QList<T> points;
      for ( int i = 0 ; i < (int) matches.size() ; i++ ) {
        points.push_back( m_pc->point(matches[i].first) );
      }

      return (points);
    }


    /** 
     * Performs a query for the nearest neighbors to a point.
     * 
     * @param point The point to find neighbors around.
     * @param neighbors The number of neighbors to find.
     * 
     * @return @b QList<T> A list of the neighbors.
     */
    QList<T> neighbor_query(const T &point, const int &neighbors) { 
      QVector<size_t> indices(neighbors);
      QVector<double> distances(neighbors);
      m_kd_index.knnSearch(point.array(), neighbors, indices.data(), 
                           distances.data());

      QList<T> points;
      for ( int i = 0 ; i < indices.size() ; i++ ) {
        points.push_back( m_pc->point(indices[i]) );

      }

      return (points);
    }


    /**
     * Returns a reference to the point cloud.
     * 
     * @return @b A PointCloud<T>& A constant reference to the point cloud.
     */
    inline const PointCloud<T> &cloud() const {
      return ( *m_pc );
    }

  private:
    QSharedPointer<PointCloud<T> >  m_pc;       //!< The point cloud.
    PcKdTree_t                      m_kd_index; //!< The kd-tree.

};

};  // namespace Isis
#endif
