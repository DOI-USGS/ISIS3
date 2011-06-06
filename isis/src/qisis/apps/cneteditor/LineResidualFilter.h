#ifndef LineResidualFilter_H
#define LineResidualFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;
  
  class LineResidualFilter : public AbstractNumberFilter
  {
      Q_OBJECT

    public:
      LineResidualFilter(AbstractFilter::FilterEffectivenessFlag flag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~LineResidualFilter();
      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;
      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
