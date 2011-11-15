#ifndef SampleResidualFilter_H
#define SampleResidualFilter_H

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
    
    class SampleResidualFilter : public AbstractNumberFilter
    {
        Q_OBJECT

      public:
        SampleResidualFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        SampleResidualFilter(const SampleResidualFilter & other);
        virtual ~SampleResidualFilter();

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
