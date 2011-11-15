#ifndef PointTypeFilter_H
#define PointTypeFilter_H

#include "AbstractMultipleChoiceFilter.h"


class QString;


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    class PointTypeFilter : public AbstractMultipleChoiceFilter
    {
        Q_OBJECT

      public:
        PointTypeFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        PointTypeFilter(const PointTypeFilter & other);
        virtual ~PointTypeFilter();

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
