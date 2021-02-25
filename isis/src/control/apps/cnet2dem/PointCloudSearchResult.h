#ifndef PointCloudSearchResult_h
#define PointCloudSearchResult_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QPair>
#include <QSharedPointer>
#include <QVector>

namespace Isis {

  template <typename T, typename D> class PointCloud;

  /**
   * @brief Point cloud query result container
   *
   * This class provides a convenient and efficent implementation to contain the
   * results of a PointCloudTree search result.
   *
   * Contained in this object is all the ControlPoints resulting from a radial or
   * neighbor proximity search.
   *
   * It is up to the calling enviroment to use the results whilst preserving the
   * content of the PointCloud elements as they are provided via a reference into
   * the original point cloud.
   *
   * Care is taken to filter out the source, as it will likely be included in
   * every search query, and any invalid points since the status of the points
   * could change during processing.
   *
   * For any type T, here are only two required methods - and operator!=() and an
   * isValid() method. Well I suppose the only other required is a default
   * constructor as well.
   *
   * @author 2015-10-11 Kris Becker
   *
   * @internal
   *   @history 2015-10-11 Kris Becker - Original Version
   */

  template <class T, class D>
  class PointCloudSearchResult {
    public:
      enum SearchType { UnDefined, Radius, NearestNeighbor };
      PointCloudSearchResult() {
        m_search_type = UnDefined;
        m_source        = T();
        m_neighbors     = 0;
        m_search_radius = 0.0;
        m_matches.clear();
        m_pc            = QSharedPointer<PointCloud<T,D> > (0);
      }

      PointCloudSearchResult(const T &source, int neighbors,
                      QVector<size_t> &indices, QVector<double> &distances,
                      QSharedPointer<PointCloud<T,D> > &pc) {
        Q_ASSERT ( indices.size() == distances.size() );
        Q_ASSERT ( neighbors == indices.size() );

        m_search_type   = NearestNeighbor;
        m_source        = source;
        m_neighbors     = neighbors;
        m_search_radius = 0.0;
        m_pc            = pc;
  //      std::cout << "kd-tree-NN: Count: " << indices.size() << "\n";
        for (int i = 0; i < indices.size(); i++ ) {
          const T &p = m_pc->point(indices[i]);
          if ( m_source != p) {
            if ( p.isValid() ) {
              m_matches.push_back(qMakePair<T, double>(p, std::sqrt(distances[i])));
            }
          }
        }
      }

      PointCloudSearchResult(const T &source, const double radius_sq,
                      std::vector< std::pair<size_t, double> > &matches,
                      const QSharedPointer<PointCloud<T,D> > &pc,
                      const int nfound = 0) {
        m_search_type = Radius;
        m_source = source;
        m_neighbors = nfound;
        m_search_radius = std::sqrt(radius_sq);
        m_pc = pc;
  //      std::cout << "kd-tree-Radial: Count: " << matches.size() << "\n";
        for (unsigned int i = 0; i < matches.size(); i++ ) {
          const T &p = m_pc->point(matches[i].first);
          if ( m_source != p) {
            if ( p.isValid() ) {
              m_matches.push_back(qMakePair<T, double>(p, std::sqrt(matches[i].second)));
            }
          }
        }
      }

      virtual ~PointCloudSearchResult() { }

      inline bool isValid() const {
        return ( !(UnDefined == m_search_type) );
      }

      inline SearchType type() const {
        return ( m_search_type );
      }

      /** Return number of points from the cloud search   */
      inline int size() const {
        return ( m_matches.size() );
      }

      inline double search_radius() const {
        return ( m_search_radius );
      }

      inline double neighbors() const {
        return ( m_neighbors );
      }

      inline const T &source() const {
        return ( m_source );
      }

      inline const T &point(const size_t idx) const {
        // Fetch the point from the PointCloud
        return ( m_pc->point(m_matches[idx].first) );
      }

      /** Return search distance in search units */
      inline double distance(const size_t idx) const {
        return ( m_matches[idx].second );
      }

      template <class P> int forEachPair( P &process ) {
          int npairs( 0 );
          for ( int m = 0; m <  m_matches.size(); m++) {
             process.apply(m_source, m_matches[m].first, m_matches[m].second);
             npairs++;
          }
          return ( npairs );
        }


    private:
      SearchType           m_search_type;
      T                    m_source;
      int                  m_neighbors;
      double               m_search_radius;

      QVector< QPair<T, double> >      m_matches;
      QSharedPointer<PointCloud<T,D> > m_pc;

  };

};  // namespace Isis
#endif
