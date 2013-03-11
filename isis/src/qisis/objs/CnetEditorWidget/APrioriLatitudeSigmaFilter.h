#ifndef APrioriLatitudeSigmaFilter_H
#define APrioriLatitudeSigmaFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by a priori surface point latitude sigma
     *
     * This class allows the user to filter control points and control measures
     * by a priori surface point latitude sigma. This allows the user to make a
     * list of control points that are less than or greater than a certain
     * a priori surface point latitude sigma.
     *
     * @author 2012-04-25 Jai Rideout
     *
     * @internal
     */
    class APrioriLatitudeSigmaFilter : public AbstractNumberFilter {
        Q_OBJECT

      public:
        APrioriLatitudeSigmaFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        APrioriLatitudeSigmaFilter(const APrioriLatitudeSigmaFilter &other);
        virtual ~APrioriLatitudeSigmaFilter();

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

