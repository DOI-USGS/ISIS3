#ifndef PointEditLockedFilter_H
#define PointEditLockedFilter_H

#include "AbstractFilter.h"


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz
  {
    class PointEditLockedFilter : public AbstractFilter
    {
        Q_OBJECT

      public:
        PointEditLockedFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        PointEditLockedFilter(const AbstractFilter & other);
        virtual ~PointEditLockedFilter();

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
