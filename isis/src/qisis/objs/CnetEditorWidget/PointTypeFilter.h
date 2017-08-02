#ifndef PointTypeFilter_H
#define PointTypeFilter_H

#include "AbstractMultipleChoiceFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

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
   */
  class PointTypeFilter : public AbstractMultipleChoiceFilter {
      Q_OBJECT

    public:
      PointTypeFilter(AbstractFilter::FilterEffectivenessFlag,
          int minimumForSuccess = -1);
      PointTypeFilter(const PointTypeFilter &other);
      virtual ~PointTypeFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
