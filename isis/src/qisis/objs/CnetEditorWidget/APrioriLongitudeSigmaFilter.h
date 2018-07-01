#ifndef APrioriLongitudeSigmaFilter_H
#define APrioriLongitudeSigmaFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class AbstractFilterSelector;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by a priori surface point longitude sigma
   *
   * This class allows the user to filter control points and control measures
   * by a priori surface point longitude sigma. This allows the user to make a
   * list of control points that are less than or greater than a certain
   * a priori surface point longitude sigma.
   *
   * @author 2012-04-25 Jai Rideout
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class APrioriLongitudeSigmaFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      APrioriLongitudeSigmaFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      APrioriLongitudeSigmaFilter(const APrioriLongitudeSigmaFilter &other);
      virtual ~APrioriLongitudeSigmaFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif

