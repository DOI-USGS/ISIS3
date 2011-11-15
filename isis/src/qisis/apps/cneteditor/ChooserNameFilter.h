#ifndef ChooserNameFilter_H
#define ChooserNameFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz
  {
    class AbstractFilterSelector;
    class ChooserNameFilter : public AbstractStringFilter
    {
        Q_OBJECT

      public:
        ChooserNameFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        ChooserNameFilter(const ChooserNameFilter & other);
        virtual ~ChooserNameFilter();

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
