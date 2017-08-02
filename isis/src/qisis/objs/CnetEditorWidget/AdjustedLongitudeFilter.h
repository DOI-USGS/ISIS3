#ifndef AdjustedLongitudeFilter_H
#define AdjustedLongitudeFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by adjusted surface point longitude
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point longitude. This allows the user to make a list
   * of control points that are less than or greater than a certain adjusted
   * surface point longitude.
   *
   * @author 2012-04-25 Jai Rideout
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class AdjustedLongitudeFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedLongitudeFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      AdjustedLongitudeFilter(const AdjustedLongitudeFilter &other);
      virtual ~AdjustedLongitudeFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif

