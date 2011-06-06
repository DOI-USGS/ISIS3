#ifndef ChooserNameFilter_H
#define ChooserNameFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  class ChooserNameFilter : public AbstractStringFilter
  {
      Q_OBJECT

    public:
      ChooserNameFilter(AbstractFilter::FilterEffectivenessFlag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~ChooserNameFilter();
      
      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
