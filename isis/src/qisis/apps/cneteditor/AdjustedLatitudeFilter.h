#ifndef AdjustedLatitudeFilter_H
#define AdjustedLatitudeFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by adjusted surface point latitude
     *
     * This class allows the user to filter control points and control measures
     * by adjusted surface point latitude. This allows the user to make a list
     * of control points that are less than or greater than a certain adjusted
     * surface point latitude.
     *
     * @author 2012-04-23 Jai Rideout
     *
     * @internal
     */
    class AdjustedLatitudeFilter : public AbstractNumberFilter {
        Q_OBJECT

      public:
        AdjustedLatitudeFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        AdjustedLatitudeFilter(const AdjustedLatitudeFilter & other);
        virtual ~AdjustedLatitudeFilter();

        bool evaluate(const ControlCubeGraphNode *) const;
        bool evaluate(const ControlPoint *) const;
        bool evaluate(const ControlMeasure *) const;

        AbstractFilter * clone() const;

        QString getImageDescription() const;
        QString getPointDescription() const;
    };
  }
}

#endif

