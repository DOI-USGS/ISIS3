#ifndef PointEditLockedFilter_H
#define PointEditLockedFilter_H

#include "AbstractFilter.h"


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlPoint;
  class ControlMeasure;

  class PointEditLockedFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      PointEditLockedFilter(AbstractFilter::FilterEffectivenessFlag flag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~PointEditLockedFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const { return true; }
      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
