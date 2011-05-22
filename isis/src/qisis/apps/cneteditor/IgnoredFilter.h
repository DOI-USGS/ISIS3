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
      
      bool canFilterImages() const;
      bool canFilterPoints() const;
      bool canFilterMeasures() const;
      
      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      QString getDescription() const;
  };
}

#endif
