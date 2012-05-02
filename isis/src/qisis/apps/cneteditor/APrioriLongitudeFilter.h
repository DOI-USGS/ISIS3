#ifndef APrioriLongitudeFilter_H
#define APrioriLongitudeFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by a priori surface point longitude
     *
     * This class allows the user to filter control points and control measures
     * by a priori surface point longitude. This allows the user to make a list
     * of control points that are less than or greater than a certain a priori
     * surface point longitude.
     *
     * @author 2012-04-25 Jai Rideout
     *
     * @internal
     */
    class APrioriLongitudeFilter : public AbstractNumberFilter {
        Q_OBJECT

      public:
        APrioriLongitudeFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        APrioriLongitudeFilter(const APrioriLongitudeFilter & other);
        virtual ~APrioriLongitudeFilter();

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

