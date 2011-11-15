#ifndef LineResidualFilter_H
#define LineResidualFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    class LineResidualFilter : public AbstractNumberFilter
    {
        Q_OBJECT

      public:
        LineResidualFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        LineResidualFilter(const LineResidualFilter & other);
        virtual ~LineResidualFilter();

        bool evaluate(const ControlCubeGraphNode *) const;
        bool evaluate(const ControlPoint *) const;
        bool evaluate(const ControlMeasure *) const;

        AbstractFilter * clone() const;

        QString getImageDescription() const;
        QString getPointDescription() const;
        QString getMeasureDescription() const;
    };
  }
}

#endif
