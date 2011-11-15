#ifndef CubeSerialNumberFilter_H
#define CubeSerialNumberFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;
  
  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    class CubeSerialNumberFilter : public AbstractStringFilter
    {
        Q_OBJECT

      public:
        CubeSerialNumberFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        CubeSerialNumberFilter(const CubeSerialNumberFilter & other);
        virtual ~CubeSerialNumberFilter();

        bool evaluate(const ControlCubeGraphNode *) const;
        bool evaluate(const ControlPoint *) const;
        bool evaluate(const ControlMeasure *) const;

        AbstractFilter * clone() const;

        QString getImageDescription() const;
        QString getPointDescription() const;
        QString getMeasureDescription() const;
    };
  }
}

#endif
