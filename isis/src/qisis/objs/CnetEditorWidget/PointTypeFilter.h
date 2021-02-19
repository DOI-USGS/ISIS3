#ifndef PointTypeFilter_H
#define PointTypeFilter_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractMultipleChoiceFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Filters by point type
   *
   * This class handles filtering by control point type (i.e. fixed,
   * constrained, free, etc.).
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *    @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class PointTypeFilter : public AbstractMultipleChoiceFilter {
      Q_OBJECT

    public:
      PointTypeFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
      PointTypeFilter(const PointTypeFilter &other);
      virtual ~PointTypeFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
