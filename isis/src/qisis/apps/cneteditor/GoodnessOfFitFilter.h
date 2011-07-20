#ifndef GoodnessOfFitFilter_H
#define GoodnessOfFitFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  class GoodnessOfFitFilter : public AbstractNumberFilter
  {
      Q_OBJECT

    public:
      GoodnessOfFitFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      GoodnessOfFitFilter(const GoodnessOfFitFilter & other);
      virtual ~GoodnessOfFitFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter * clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
