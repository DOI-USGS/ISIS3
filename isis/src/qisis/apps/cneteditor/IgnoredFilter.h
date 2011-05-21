#ifndef IgnoredPointsFilter_H
#define IgnoredPointsFilter_H

#include "AbstractPointMeasureFilter.h"


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;
  class ControlCubeGraphNode;

  class IgnoredFilter : public AbstractPointMeasureFilter
  {
      Q_OBJECT

    public:
      IgnoredFilter(int minimumForImageSuccess = -1);
      virtual ~IgnoredFilter();

      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;
      bool evaluate(const ControlCubeGraphNode *) const;
      QString getDescription() const;
  };
}

#endif
