#ifndef MeasureJigsawRejectedFilter_H
#define MeasureJigsawRejectedFilter_H

#include "AbstractFilter.h"


namespace Isis {
  class ControlCubeGraphNode;
  class ControlMeasure;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Allows filtering by a control measure's jigsaw rejected status
     *
     * This class allows the user to filter control measures based on whether or
     * not they are rejected by jigsaw. This allows the user to make a list of
     * jigsaw rejected or not-jigsaw rejected control measures.
     *
     * @author 2012-04-18 Jai Rideout
     *
     * @internal
     */
    class MeasureJigsawRejectedFilter : public AbstractFilter {
        Q_OBJECT

      public:
        MeasureJigsawRejectedFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        virtual ~MeasureJigsawRejectedFilter();

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

