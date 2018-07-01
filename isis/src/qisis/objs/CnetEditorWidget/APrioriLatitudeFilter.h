#ifndef APrioriLatitudeFilter_H
#define APrioriLatitudeFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by a priori surface point latitude
   *
   * This class allows the user to filter control points and control measures
   * by a priori surface point latitude. This allows the user to make a list
   * of control points that are less than or greater than a certain a priori
   * surface point latitude.
   *
   * @author 2012-04-25 Jai Rideout
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class APrioriLatitudeFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      APrioriLatitudeFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      APrioriLatitudeFilter(const APrioriLatitudeFilter &other);
      virtual ~APrioriLatitudeFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif

