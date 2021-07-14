/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ControlPointMerger.h"

#include <boost/foreach.hpp>

namespace Isis {

  /**
    * Constructs a default ControlPointMerger.
    * The tolerance defaults to DBL_MAX.
    */
  ControlPointMerger::ControlPointMerger() : m_image_tolerance(DBL_MAX), m_merged() { }


  /**
    * Constructs a ControlPointMerger with a given tolerance.
    *
    * @param image_tolerance The tolerance used to determine if points will be merged.
    */
  ControlPointMerger::ControlPointMerger(const double image_tolerance) :
                      m_image_tolerance(image_tolerance), m_merged() { }


  /**
    * Destroys a ControlPointMerger.
    */
  ControlPointMerger::~ControlPointMerger() { }


  /**
    * Returns the number of points that have been merged.
    *
    * @return @b int The number of points that have been merged.
    */
  int ControlPointMerger::size() const {
    return ( m_merged.size() );
  }


  /**
   * Returns a set of the IDs of the merged control points
   */
  QSet<QString> ControlPointMerger::mergedPoints() const {
    QSet<QString> mergedIds;
    BOOST_FOREACH ( const MeasurePoint &measure, m_merged ) {
      mergedIds.insert(measure.id());
    }
    return mergedIds;
  }


  /**
    * Clears the list of merged points.
    */
  void ControlPointMerger::clear() {
    m_merged.clear();
    return;
  }


  /**
  * @brief Merges control points that satisfy image coordinate constraints
  *
  * Evaluates a list of candidate points to determine if they should be merged
  * into a source point.  For each candidate point, the image space distance
  * between its measures and the source point's measures on shared images are
  * averaged.  If this average is less than the ControlPointMerger's tolerance
  * the candidate point is merged into the source point.
  *
  * @author 2016-10-25 Kris Becker
  *
  * @param point       Source point to merge candidates into
  * @param candidates  List of point candidates
  *
  * @return @b int     Number of measures merged
  */
  int ControlPointMerger::apply(ControlPoint *point, QList<MeasurePoint> &candidates)  {

    QList<ControlMeasure *> measures = point->getMeasures( true );
    QList<ControlPoint *> v_processed;
    clear();

    int nMerged(0);
    BOOST_FOREACH ( MeasurePoint &measure, candidates ) {
      ControlPoint *v_p = measure.getPoint();
      if ( point != v_p) {  // Don't process the same point!
        if (measure.isValid() && (!v_processed.contains(v_p))) {
          v_processed.append(v_p);

          // Compute distance statistics of common measures in two points
          Statistics stats;
          BOOST_FOREACH ( ControlMeasure *m, measures ) {
            if ( v_p->HasSerialNumber( m->GetCubeSerialNumber() ) ) {
              ControlMeasure *c = v_p->GetMeasure( m->GetCubeSerialNumber() );
              if ( isValid(*c) ) {
                stats.AddData( image_distance(*m, *c) );
              }
            }
          }

          // Test for conditions of a merger. If there are common image measures,
          // use the statistics.
          if ( stats.ValidPixels() > 0 ) {
            if (stats.Average() <= m_image_tolerance) {
              nMerged += merge(point, v_p, stats);
              measure.disable();
              m_merged.append(measure);
            }
          }
        }
      }
    }
    return (nMerged);
  }


  /**
    * @brief Merges measures from one control point into another.
    *
    * Merges measures on shared images from a candidate point into a source
    * point.  If a reference measure is merged, then its residual and goodness
    * of fit are set to the average and standard deviation of the distance
    * between the source and candidate points' measures.
    *
    * @param source     The source point that measueres are merged into.
    * @param candidate  The candidate point that measures are merged from.
    * @param stats      Statistics object containing the distances between
    *                   measures in the source and candidate points on
    *                   common images.
    *
    * @return @b int    The number of measures merged from the candidate point
    *                   into the source point.
    *
    * @see ControlPointMerger::apply
    */
  int ControlPointMerger::merge(ControlPoint *source, ControlPoint *candidate,
                                const Statistics &stats) {
    int nMerged(0);
    bool setCandidateType = true;

    // Gut check to ensure we don't merge into ourself
    if ( source != candidate ) {

      // Set up for merging a reference measure. Expectation is the tolerance
      // is <= 1, but don't assume that.
      double residual = stats.Average();
      double goodnessoffit = qMin(qMax(0.0, stats.StandardDeviation()), 1.0);
      ControlMeasureLogData data(ControlMeasureLogData::GoodnessOfFit,
                                  goodnessoffit);

      // Consider only valid measures
      QList<ControlMeasure*> measures = candidate->getMeasures(true);

      //Check to see if reference measures for both source/candidate are the same.
      if (source->GetRefMeasure() == candidate->GetRefMeasure()) {


        setCandidateType = false;

      }


      BOOST_FOREACH ( ControlMeasure *m, measures ) {
        if ( !source->HasSerialNumber(m->GetCubeSerialNumber()) ) {
          QScopedPointer<ControlMeasure> p_m(new ControlMeasure(*m));

          // Handle the transfer of a reference measure from the candidate
          if ( candidate->GetRefMeasure() == m ) {
            p_m->SetResidual(residual, residual);
            p_m->SetLogData(data);


          }
          if (setCandidateType) {
            p_m->SetType(ControlMeasure::Candidate);
          }
          source->Add( p_m.take() );
          nMerged++;
        }
      }
      // Essentially disables this point meaning this point is already merged
      // with another
      candidate->SetIgnored(true);
    }

    return ( nMerged );
  }


  /**
    * Determines if a measure is valid.
    *
    * @param m The measure to check.
    *
    * @return @b bool If the measure is ignored or rejected.
    */
  bool ControlPointMerger::isValid(const ControlMeasure &m) const {
      return ( !( m.IsIgnored() || m.IsRejected() ) );
  }

  /**
    * Computes the distance, in image, between two measures.
    * Note that this returns the squared distance because that
    * is what the nanoflann library expects.
    *
    * @param source      A measure from the source point.
    * @param candidate   A measure from the candidate point.
    *
    * @return @b double  The squared distance between the measures.
    */
  inline double ControlPointMerger::image_distance(const ControlMeasure &source,
                                                   const ControlMeasure &candidate) const {
    double dx = source.GetSample() - candidate.GetSample();
    double dy = source.GetLine() - candidate.GetLine();
    return ( std::sqrt( dx*dx + dy*dy ) );
  }

}  // namespace Isis
