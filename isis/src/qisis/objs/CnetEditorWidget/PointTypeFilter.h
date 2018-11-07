#ifndef PointTypeFilter_H
#define PointTypeFilter_H

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
