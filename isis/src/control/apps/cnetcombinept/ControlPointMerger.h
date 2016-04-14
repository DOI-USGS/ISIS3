#ifndef ControlPointMerger_h
#define ControlPointMerger_h
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
#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

#include "ControlPointCloudPt.h"
#include "Statistics.h"

namespace Isis {

/**
 * @brief Combine control points based upon distance criteria 
 *  
 * This class will collect and compute ControlPoint candidates that are within 
 * a pixel tolerance for merging into a single control point. 
 *  
 * The criteria applied computes statistics on all common measures within the 
 * control point for image coordinate searches.
 *  
 * @author  2015-10-11 Kris Becker
 *  
 * @internal 
 *   @history 2015-10-11 Kris Becker - Original Version 
 */

class ControlPointMerger {
  public:
    ControlPointMerger() : m_image_tolerance(DBL_MAX),
                           m_ground_tolerance(DBL_MAX),
                           m_ground_distance(DBL_MAX),
                           m_source(), m_candidates() { }
    ControlPointMerger(const double image_tolerance,
                       const double ground_tolerance = -1.0) :
                       m_image_tolerance(image_tolerance),
                       m_ground_tolerance(ground_tolerance),
                       m_ground_distance(DBL_MAX), 
                       m_source(), m_candidates() { }

    virtual ~ControlPointMerger() { }

    int size() const {
      return ( m_candidates.size() );
    }

    Statistics getImageStatistics(int index) const {
      Q_ASSERT( index >= 0 );
      Q_ASSERT ( index < size() );
      return ( m_candidates[index].second );
    }

    double getGroundDistance() const {
      return ( m_ground_distance );
    }

    void clear() {
      m_candidates.clear();
      return;
    }

    void apply(ControlPointCloudPt &source, ControlPointCloudPt &candidate,
               const double distance)  {

      if ( !source.isValid() ) { return; }
      if ( !candidate.isValid() ) { return; }
      m_source = source;

      ControlPoint &point = source.getPoint();
      m_ground_distance = ground_distance( point, candidate.getPoint() );

      QList<ControlMeasure *> measures = point.getMeasures( true );
      Statistics stats;
      BOOST_FOREACH ( ControlMeasure *m, measures ) {
        if ( isValid(*m) ) {
          ControlMeasure *c = candidate.getMeasure(m->GetCubeSerialNumber()); 
          if ( (0 != c) ) {  
            if ( isValid(*c) ) { stats.AddData(image_distance(*m, *c)); }
          }
        }
      }

      // Test for conditions of a merger. If there are common image measures,
      // use the statistics. If no common measures, use the ground distance.
      if ( stats.ValidPixels() > 0 ) {
        if (stats.Average() <= m_image_tolerance) {
          m_candidates.append( qMakePair<ControlPointCloudPt, Statistics> (candidate, stats)); 
        }
      }
      else if ( m_ground_distance <= m_ground_tolerance ) {
         m_candidates.append( qMakePair<ControlPointCloudPt, Statistics> (candidate, stats)); 
      }
      return;
    }

    int merge() {
      int nMerged(0);
      ControlPoint &source = m_source.getPoint();
      for ( int i = 0 ; i < m_candidates.size() ; i++) {
        ControlPointCloudPt &cpt = m_candidates[i].first;
        ControlPoint &candidate = cpt.getPoint();
        QList<ControlMeasure *> ms = candidate.getMeasures(true);
        for ( int m = 0 ; m < ms.size() ; m++) {
          if ( !source.HasSerialNumber(ms[m]->GetCubeSerialNumber()) ) {
            source.Add( new ControlMeasure(*ms[m]) );
            nMerged++;
          }
        }
        // Essentially disables this point
        cpt.disable();
      }

      return ( nMerged );
    }

  private:
    
    double                                         m_image_tolerance;
    double                                         m_ground_tolerance;
    double                                         m_ground_distance;
    ControlPointCloudPt                            m_source;
    QList<QPair<ControlPointCloudPt, Statistics> > m_candidates;

    inline bool isValid(const ControlMeasure &m) const {
       return ( !( m.IsIgnored() || m.IsRejected() ) );
    }

    inline double image_distance(const ControlMeasure &source, 
                                 const ControlMeasure &candidate) const {
      double dx = source.GetSample() - candidate.GetSample();
      double dy = source.GetLine() - candidate.GetLine();
      return ( std::sqrt( dx*dx + dy*dy ) );
    }

    inline double ground_distance(const ControlPoint &source, 
                                  const ControlPoint &candidate) const {
      double spts[3], cpts[3];
      getGroundVector(source, spts);
      getGroundVector(candidate, cpts);

      double dx = spts[0] - cpts[0];
      double dy = spts[1] - cpts[1];
      double dz = spts[2] - cpts[2];
      return ( std::sqrt( dx*dx + dy*dy + dz*dz ) );
    }

    /** Compute ground point in meters */
    inline void getGroundVector(const ControlPoint &point, double v[3]) const {
      // Always use the best surface point available
       point.GetBestSurfacePoint().ToNaifArray(v);
       v[0] *= 1000.0;
       v[1] *= 1000.0;
       v[2] *= 1000.0;
       return;
    }
};

}  // namespace Isis
#endif
