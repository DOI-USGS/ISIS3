#ifndef LineShiftFilter_H
#define LineShiftFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by a control measure's line shift
     *
     * This class allows the user to filter control measures by their line shift
     * (i.e. how many lines they shifted in the image). This allows the user to
     * make a list of control measures that shifted by a certain amount in an
     * image after adjustment. The line shift is the difference between the
     * measure's line and a priori line.
     *
     * @author 2012-04-18 Jai Rideout
     *
     * @internal
     */
    class LineShiftFilter : public AbstractNumberFilter {
        Q_OBJECT

      public:
        LineShiftFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        LineShiftFilter(const LineShiftFilter &other);
        virtual ~LineShiftFilter();

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

