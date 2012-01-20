#ifndef MeasureTypeFilter_H
#define MeasureTypeFilter_H

#include "AbstractMultipleChoiceFilter.h"


class QString;


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    /**
     * @brief Filters by measure type
     *
     * This class handles filtering by control measure type (i.e. candidate,
     * manual, registered pixel, registered subpixel, etc.). This can be used to
     * generate a list of control points that have a minimum number of control
     * measures of a certain type.
     *
     * @author 2012-01-05 Jai Rideout
     *
     * @internal
     */
    class MeasureTypeFilter : public AbstractMultipleChoiceFilter
    {
        Q_OBJECT

      public:
        MeasureTypeFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        MeasureTypeFilter(const MeasureTypeFilter & other);
        virtual ~MeasureTypeFilter();

        bool evaluate(const ControlCubeGraphNode *) const;
        bool evaluate(const ControlPoint *) const;
        bool evaluate(const ControlMeasure *) const;

        AbstractFilter * clone() const;

        QString getImageDescription() const;
        QString getMeasureDescription() const;
        QString getPointDescription() const;
    };
  }
}

#endif

