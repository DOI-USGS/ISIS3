#ifndef PointIgnoredFilter_H
#define PointIgnoredFilter_H

#include "AbstractFilter.h"


namespace Isis
{
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;
  
  class PointIgnoredFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      PointIgnoredFilter(AbstractFilter::FilterEffectivenessFlag flag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~PointIgnoredFilter();
      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure * ) const { return true; }
      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
