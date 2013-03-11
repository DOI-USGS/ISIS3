#ifndef SampleFilter_H
#define SampleFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by a control measure's sample
     *
     * This class allows the user to filter control measures by their sample
     * (i.e. which sample they are positioned at in the image). This allows the
     * user to make a list of control measures that are too close to the edge of
     * an image after pointreg adjustment.
     *
     * @author 2012-01-05 Jai Rideout
     *
     * @internal
     */
    class SampleFilter : public AbstractNumberFilter {
        Q_OBJECT

      public:
        SampleFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        SampleFilter(const SampleFilter &other);
        virtual ~SampleFilter();

        bool evaluate(const ControlCubeGraphNode *) const;
        bool evaluate(const ControlPoint *) const;
        bool evaluate(const ControlMeasure *) const;

        AbstractFilter *clone() const;

        QString getImageDescription() const;
        QString getPointDescription() const;
        QString getMeasureDescription() const;
    };
  }
}

#endif

