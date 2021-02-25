#ifndef CnetManager_h
#define CnetManager_h

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
#include <QList>
#include <QMap>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QtCore/qmath.h>
#include <QVector>

#include <boost/assert.hpp>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"


#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array2d_utils.h"

namespace Isis {

  class CnetManager;

/**
 * Class to store control points with a weight and computed strength for CnetManager.
 *
 * @author 2016-09-30 Kris Becker
 *
 * @internal
 *   @history 2016-09-30 Kris Becker - Original Version
 *   @history 2016-12-28 Kristin Berry - Added documentation and tests for checkin
 *
 */
  class KPoint {

    friend class CnetManager;

    public:
      KPoint();
      KPoint(ControlPoint *point, const int &index, const double &weight = 0.0);

      void select(const bool &state = true);

      //definitions of inline functions

     /**
      * Get the control point the KPoint was constructed from.
      *
      * @return @b ControlPoint* The original control point.
      */
      inline ControlPoint *point() const {
        BOOST_ASSERT ( m_point != 0 );
        return ( m_point );
      }


     /**
      * Gets the calculated strength of this KPoint.
      *
      * @return @b double The calculated strength of this KPoint.
      */
      inline double strength() const {
        return ( m_strength );
      }


     /**
      * Gets the index of this KPoint.
      *
      * @return @b int The index of this KPoint.
      */
      inline int index () const {
        return ( m_index );
      }


     /**
       * Gets the original index of this KPoint.
       *
       * @return @b int The original or source index of this KPoint.
       */
      inline int sourceIndex() const {
        return (m_sourceIndex);
      }

  private:
      ControlPoint *m_point; //! The original ControlPoint used to construct the KPoint.
      double        m_strength; //! The calulated strength of this KPoint.
      int           m_sourceIndex; //! The original index of this KPoint.
      int           m_index; //! The calculated index of this KPoint.
      bool          m_selected; //! Flag to indicated whether to use this KPoint or not.

      double calculateStrength(const ControlPoint *point, const double &weight) const;

  };


/**
 * Container class for the network and suppression data.
 *
 * @author 2016-09-30 Kris Becker
 *
 * @internal
 *   @history 2016-09-30 Kris Becker - Original Version
 *   @history 2016-12-28 Kristin Berry - Added documentation and tests for checkin
 */
  class CnetManager {
    public:
      typedef QPair<int, ControlMeasure *> IndexPoint; //!
      typedef QVector<IndexPoint>          PointSet; //!

      CnetManager();
      CnetManager(ControlNet &cnet, const double &weight = 0.0);
      virtual ~CnetManager();

      int size() const;
      int load(const QList<ControlPoint *> &pts, const double &weight = 0.0);

      QMap<QString, int> getCubeMeasureCount() const;
      const QList<ControlPoint *> getControlPoints() const;
      PointSet getCubeMeasureIndices(const QString &serialNo) const;

      const KPoint &operator()(const int index) const;
      const ControlPoint *point(const int &index) const;
      const QList<KPoint> &pointList() const;

    protected:
      ControlPoint *point(const int index);

    private:
      QList<KPoint>              m_kpts; //! List of KPoints managed by this class

      /**
       * @brief Ascending order sort functor
       *
       * This is a comparison class used to sort lists of KPoint objects by strength, in
       * ascending order.
       *
       * @author 2016-09-30 Kris Becker
       *
       * @internal
       *   @history 2016-09-30 Kris Becker - Original Version
       *
       */
      class SortStrengthDescending {
        public:
          SortStrengthDescending() { }
          ~SortStrengthDescending() { }
          inline bool operator()(const KPoint &a, const KPoint &b) const {
            return  ( a.strength() >  b.strength() );
          }
      };


  };


}  // namespace Isis
#endif
