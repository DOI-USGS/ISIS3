#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis {
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz {
    class AbstractFilterSelector;

    /**
     * @brief Filter by control point id string
     *
     * This class allows the user to filter control points based on what the
     * control point id is. This allows the user to find a particular control
     * point or make a list of control points with similar ids.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class PointIdFilter : public AbstractStringFilter {
        Q_OBJECT

      public:
        PointIdFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        PointIdFilter(const PointIdFilter &other);
        virtual ~PointIdFilter();

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
