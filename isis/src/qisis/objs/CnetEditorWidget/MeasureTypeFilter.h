#ifndef MeasureTypeFilter_H
#define MeasureTypeFilter_H

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
   * @brief Filters by measure type
   *
   * This class handles filtering by control measure type (i.e. candidate,
   * manual, registered pixel, registered subpixel, etc.). This can be used to
   * generate a list of control points that have a minimum number of control
   * measures of a certain type.
   *
   * @author 2012-01-05 Jai Rideout
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class MeasureTypeFilter : public AbstractMultipleChoiceFilter {
      Q_OBJECT

    public:
      MeasureTypeFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
      MeasureTypeFilter(const MeasureTypeFilter &other);
      virtual ~MeasureTypeFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getMeasureDescription() const;
      QString getPointDescription() const;
  };
}

#endif
