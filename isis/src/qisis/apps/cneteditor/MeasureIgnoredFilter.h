#ifndef MeasureIgnoredFilter_H
#define MeasureIgnoredFilter_H

#include "AbstractFilter.h"


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlMeasure;
  
  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    /**
     * @brief Allows filtering by a control measure's ignored status
     *
     * This class allows the user to filter control measures based on whether or
     * not they are ignored. This allows the user to make a list of ignored or
     * not-ignored control measures.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class MeasureIgnoredFilter : public AbstractFilter
    {
        Q_OBJECT

      public:
        MeasureIgnoredFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
        virtual ~MeasureIgnoredFilter();

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
