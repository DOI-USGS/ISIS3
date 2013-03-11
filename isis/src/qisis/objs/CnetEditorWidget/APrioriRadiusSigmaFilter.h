#ifndef APrioriRadiusSigmaFilter_H
#define APrioriRadiusSigmaFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by a priori surface point radius sigma
     *
     * This class allows the user to filter control points and control measures
     * by a priori surface point radius sigma. This allows the user to make a
     * list of control points that are less than or greater than a certain
     * a priori surface point radius sigma.
     *
     * @author 2012-04-25 Jai Rideout
     *
     * @internal
     */
    class APrioriRadiusSigmaFilter : public AbstractNumberFilter {
        Q_OBJECT

      public:
        APrioriRadiusSigmaFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        APrioriRadiusSigmaFilter(const APrioriRadiusSigmaFilter &other);
        virtual ~APrioriRadiusSigmaFilter();

        bool evaluate(const ControlCubeGraphNode *) const;
        bool evaluate(const ControlPoint *) const;
        bool evaluate(const ControlMeasure *) const;

        AbstractFilter *clone() const;

        QString getImageDescription() const;
        QString getPointDescription() const;
    };
  }
}

#endif

