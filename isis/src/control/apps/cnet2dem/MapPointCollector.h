#ifndef MapPointCollector_h
#define MapPointCollector_h
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

#include <QList>
#include <QPair>
#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

#include "ControlPointCloudPt.h"
#include "Statistics.h"

namespace Isis {

  /**
   * @brief Gather ControlPoints for generation of an output map (radius) pixel
   *  
   * This class will collect ControlPoint candidates that are within 
   * a tolerance generation of an output radius map pixel
   *  
   * @author 2015-11-16 Kris Becker 
   *  
   * @internal 
   *   @history 2015-11-16 Kris Becker - Original Version
   */
  
  class MapPointCollector {
    public:
      enum SearchType { UnDefined, Radius, NearestNeighbor };
      MapPointCollector() : m_source(), m_candidates(),
                            m_radius_stats(), m_distance_stats() { }
      virtual ~MapPointCollector() { }
  
      int size() const {
        return ( m_candidates.size() );
      }
  
      const Statistics &getRadiusStatistics() const {
        return ( m_radius_stats );
      }
  
      const Statistics &getDistanceStatistics() const {
        return ( m_distance_stats );
      }
  
      const ControlPointCloudPt &getSource() const {
        return ( m_source );
      }
  
      const ControlPointCloudPt &getPoint(const int index) const {
        Q_ASSERT (index >= 0);
        Q_ASSERT ( index < m_candidates.size() );
        return ( m_candidates[index].first );
      }
  
      double getDistance(const int index) const {
        Q_ASSERT (index >= 0);
        Q_ASSERT ( index < m_candidates.size() );
        return ( m_candidates[index].second );
      }
  
      SearchType getSearchType() const {
        return ( m_search_type );
      }
  
      void setSearchType (const SearchType stype) {
        m_search_type = stype;
      }
  
      void apply(ControlPointCloudPt &source, ControlPointCloudPt &candidate,
                 const double distance)  {
  #if 0
        if ( !source.isValid() ) { return; }
        if ( !candidate.isValid() ) { return; }
  #endif
  
        m_source = source;
        m_candidates.push_back( qMakePair(candidate, distance));
        m_radius_stats.AddData(candidate.radius());
        m_distance_stats.AddData(distance);
        return;
      }
  
    /**
     * @brief Noise remove using standard deviations with radius
     * 
     * @author 2015-12-10 Kris Becker
     * 
     * @param sigma Number of standard deviations from the median to filter as 
     *              noise
     * 
     * @return int number removed
     */
      int removeNoise(const double sigma = 3.0) {
         if ( size() < 3 ) return (0);  // Can't really do with less
  
         double v_median = computeMedian(m_candidates);
         double v_stddev = m_radius_stats.StandardDeviation();
         double v_tolerance = v_stddev * sigma;
  
         PointDistPair v_outpairs;
         Statistics    v_radiusStats, v_distanceStats;
         for (int i = 0; i < m_candidates.size(); i++) {
           double v_radius = m_candidates[i].first.radius();
           double v_range = qAbs( v_radius - v_median);
           if ( v_range <= v_tolerance)  {
             v_outpairs.append(m_candidates[i]);
             v_radiusStats.AddData(v_radius);
             v_distanceStats.AddData(m_candidates[i].second);
           }
         }
  
         // Recompute statistics
         int v_removed = m_candidates.size() - v_outpairs.size();
         m_candidates = v_outpairs;
         m_radius_stats = v_radiusStats;
         m_distance_stats = v_distanceStats;
        return ( v_removed );
      }
  
    private:
      typedef QList< QPair<ControlPointCloudPt, double> >  PointDistPair;
      ControlPointCloudPt  m_source;
      PointDistPair        m_candidates;
      Statistics           m_radius_stats;
      Statistics           m_distance_stats;
      SearchType           m_search_type;
  
      double computeMedian(const PointDistPair &points) const {
        double median = Null;
        if ( points.size() == 0 )  return ( median );
        if ( points.size() == 1 )  return ( points[0].first.radius() );
  
        QVector<double> v_radii;
        for (int i = 0; i < points.size(); i++) {
          v_radii.push_back(points[i].first.radius() );
        }
  
        qSort(v_radii);
        if ( (v_radii.size() % 2) == 0 ) {
          int ndx0 = (v_radii.size() - 1) / 2;
          median = (v_radii[ndx0] + v_radii[ndx0+1]) / 2.0;
        }
        else {
          median = v_radii[v_radii.size()/2];
        }
        return ( median ); 
      }
  };

}  // namespace Isis
#endif
