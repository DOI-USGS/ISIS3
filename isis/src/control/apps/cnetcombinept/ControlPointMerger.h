#ifndef ControlPointMerger_h
#define ControlPointMerger_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QtGlobal>
#include <QList>
#include <QSet>
#include <QString>
#include <QScopedPointer>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>
#include <boost/assert.hpp>

#include "ControlMeasureLogData.h"
#include "MeasurePoint.h"
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
 *   @history 2016-12-06 Jesse Mapel - Updated documentation.  References #4558.
 *   @history 2016-12-06 Jesse Mapel - Moved implementation to cpp file.  References #4558.
 */

class ControlPointMerger {
  public:

    ControlPointMerger();
    ControlPointMerger(const double image_tolerance);
    virtual ~ControlPointMerger();

    int size() const;
    QSet<QString> mergedPoints() const;
    void clear();
    int apply(ControlPoint *point, QList<MeasurePoint> &candidates);
    int merge(ControlPoint *source, ControlPoint *candidate, const Statistics &stats);

  private:
    double               m_image_tolerance; /**!< The image distance tolerance for determining
                                                  if control points should be merged.*/
    QList<MeasurePoint>  m_merged;          /**!< The number of control points that have been
                                                  merged by the ControlPointMerger.*/

    inline bool isValid(const ControlMeasure &m) const;
    inline double image_distance(const ControlMeasure &source,
                                 const ControlMeasure &candidate) const;
};

}  // namespace Isis
#endif
