#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  class PointIdFilter : public AbstractStringFilter
  {
      Q_OBJECT

    public:
      PointIdFilter(AbstractFilter::FilterEffectivenessFlag,
          int minimumForSuccess = -1);
      PointIdFilter(const PointIdFilter & other);
      virtual ~PointIdFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter * clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
