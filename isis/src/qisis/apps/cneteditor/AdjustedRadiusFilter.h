#ifndef AdjustedRadiusFilter_H
#define AdjustedRadiusFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by adjusted surface point radius
     *
     * This class allows the user to filter control points and control measures
     * by adjusted surface point radius. This allows the user to make a list
     * of control points that are less than or greater than a certain adjusted
     * surface point radius.
     *
     * @author 2012-04-25 Jai Rideout
     *
     * @internal
     */
    class AdjustedRadiusFilter : public AbstractNumberFilter {
        Q_OBJECT

      public:
        AdjustedRadiusFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        AdjustedRadiusFilter(const AdjustedRadiusFilter & other);
        virtual ~AdjustedRadiusFilter();

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

