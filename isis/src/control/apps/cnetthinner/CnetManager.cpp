/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <ostream>
#include <cassert>
#include <cfloat>
#include <cmath>

#include <QtAlgorithms>
#include <QMap>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QtCore/qmath.h>

#include <boost/foreach.hpp>

#include "CnetManager.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"

#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array2d_utils.h"

///#define DEBUG 1

namespace Isis {

// KPoint implementation

/**
 * Empty Constructor. Disabled at runtime.
 *
 */
  KPoint::KPoint() {
    m_point = 0;
    m_strength = -3;
    m_index = -1;
    m_selected = false;
    BOOST_ASSERT ( m_point != 0 );
  }


/**
 * Constructs a KPoint.
 *
 * @param point The control point to make a KPoint from.
 * @param index The source index.
 * @param weight The point's weight.
 *
 */
  KPoint::KPoint(ControlPoint *point, const int &index, const double &weight) {
    BOOST_ASSERT ( point != 0 );
    m_point = point;
    m_strength = calculateStrength(m_point, weight);
    m_sourceIndex = m_index = index;
    m_selected = false;
  }


/**
 * Set the status of the KPoint to true for selected or
 * false for unselected.
 *
 * @param state If true, the KPoint is set to selected.
 */
  void KPoint::select(const bool &state) {
    m_selected = state;
  }


/**
 * Calculate the strength of a control point. A negative return value indicates
 * an invalid result.
 *
 * @param point
 * @param weight
 *
 * @return double
 */
  double KPoint::calculateStrength(const ControlPoint *point,
                           const double &weight) const {
    // Gut check for validity
    if ( !point )  return (-2.0);

    // Don't use points which have only 1 valid measure or fewer, since
    // we don't use refIncides in the strength calculation.
    if ( point->GetNumValidMeasures() < 2.0 ) return (-1.0);

    // Got some good ones, compute strength
    double sum(0.0);
    int count(0);
    int refIndex = point->IndexOfRefMeasure();
    for ( int i = 0 ; i < point->GetNumMeasures() ; i++) {
      if ( i != refIndex ) { // Check all but the reference measure
        const ControlMeasure *m = point->GetMeasure(i);
        if ( !m->IsIgnored() && m->HasLogData(ControlMeasureLogData::GoodnessOfFit) ) {
          sum += m->GetLogData(ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();
          count++;
        }
      }
    }

    // Compute the weighted strength
    BOOST_ASSERT ( count > 0 );
    double v_count(count);
    double v_strength = sum / v_count;
    return ( v_strength * ( 1.0 + (qLn(v_count) * weight) ) );
  }

// CnetManager implementation

/**
 * Constructs an emtpy CnetManager.
 *
 */
  CnetManager::CnetManager() :  m_kpts() { }


/**
 * Constructs a CnetManager using an input control network and a weight.
 *
 * @param cnet Input control network to be managed.
 * @param weight Weights to apply to the control network.
 */
  CnetManager::CnetManager(ControlNet &cnet, const double &weight) {
    load(cnet.GetPoints(), weight);
  }


/**
 *
 * Default Destructor
 *
 */
  CnetManager::~CnetManager() { }


/**
 * The number of points managed by CnetManager.
 *
 * @return @b int The number of points in this CnetManager.
 */
   int CnetManager::size() const {
    return ( m_kpts.size() );
  }


/**
 * Loads a list of control points into the CnetManager.
 *
 * @param pts The QList of ControlPoints to load into the CnetManager.
 * @param weight The weight to apply to each ControlPoint in the list.
 *
 * @return @b int The number of ControlPoints loaded into the CnetManager.
 */
   int CnetManager::load(const QList<ControlPoint *> &pts, const double &weight) {
    m_kpts.clear();

    for (int i = 0; i < pts.size(); i++) {
      ControlPoint *p = pts[i];
      if ( !(p->IsInvalid() || p->IsIgnored()) ) {
        m_kpts.append(KPoint(p, i, weight));
      }
    }

    // Sort the points based upon strength
    qSort(m_kpts.begin(), m_kpts.end(), SortStrengthDescending());

#if defined(DEBUG)
    for (int p = 0 ; p < qMin(m_kpts.size(), 5) ; p++) {
      std::cout << "Point: " << p << " - Strength: " << m_kpts[p].strength() << "\n";
    }
#endif

    // Now have to set the real sorted indexes
    for (int i = 0; i < m_kpts.size(); i++) {
      m_kpts[i].m_index = i;
    }

    return ( size() );
  }


/**
 * Get a list of control points in this CnetManager.
 *
 * @return @b const QList<ControlPoint*>  List of control points in CnetManager.
 */
   const QList<ControlPoint *> CnetManager::getControlPoints() const {
    QList<ControlPoint *> pts;
    BOOST_FOREACH ( const KPoint &p, m_kpts ) {
      pts.append( p.point() );
    }
    return (pts);
  }


/**
 * Return a map of the number of measures per cube.
 *
 * @return @b QMap<QString,int> Serial number, number of measures per that serial number (cube)
 */
  QMap<QString, int> CnetManager::getCubeMeasureCount() const {
    QMap<QString, int> k_map;
    BOOST_FOREACH ( const KPoint &p, m_kpts ) {
      QList<ControlMeasure *> measures = p.point()->getMeasures(true);
      BOOST_FOREACH ( const ControlMeasure *m, measures ) {
        QString cn = m->GetCubeSerialNumber();
        if ( k_map.contains(cn) ) { k_map[cn] += 1; }
        else                      { k_map.insert(cn, 1); }
      }
    }
    return (k_map);
  }


/**
 * Returns control measures and their associated indicies for a given cube (serial number.)
 *
 * @param serialNo Serial number (indicates a specific cube) to get the measure indicies for.
 *
 * @return @b CnetManager::PointSet Data structure containing the calculated index and measure.
 */
  CnetManager::PointSet CnetManager::getCubeMeasureIndices(const QString &serialNo) const {
    PointSet cubeNdx;
    BOOST_FOREACH ( const KPoint &p, m_kpts ) {
      int index = p.point()->IndexOf(serialNo, false);
      if ( index >= 0 ) {
        cubeNdx.append( qMakePair(p.index(), p.point()->GetMeasure(index)) );
      }
    }
    return ( cubeNdx );
  }


/**
 * Will return the KPoint at an input index.
 *
 * @return @b const KPoint& The KPoint at the requested index.
 */
  const KPoint &CnetManager::operator()(const int index) const {
    BOOST_ASSERT ( index < m_kpts.size() );
    return ( m_kpts.at(index) );
  }


/**
 *
 * Get a point at a specificed index.
 *
 * @param index Index of the point to get.
 *
 * @return @b const ControlPoint* The point at the requested index.
 */
  const ControlPoint *CnetManager::point(const int &index) const {
    BOOST_ASSERT ( index < m_kpts.size() );
    return ( m_kpts.at(index).point() );
  }


/**
 * Gets the list of KPoints managed by this CubeManager.
 *
 * @return @b const QList<KPoint>&  List of KPoints managed by this CnetManager.
 */
  const QList<KPoint> &CnetManager::pointList() const {
    return ( m_kpts );
  }


/**
 * Get a point at a specificed index.
 *
 * @param index Index of the point.
 *
 * @return @b ControlPoint* The point at the requested index.
 */
  ControlPoint *CnetManager::point(const int index) {
    BOOST_ASSERT ( index < m_kpts.size() );
    return ( m_kpts.at(index).point() );
  }

}  // namespace Isis
