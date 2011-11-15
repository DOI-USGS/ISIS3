#ifndef MeasureCountFilter_H
#define MeasureCountFilter_H


// parent
#include "AbstractFilter.h"


class QButtonGroup;
class QLineEdit;
class QSpinBox;
class QString;


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    class MeasureCountFilter : public AbstractFilter
    {
        Q_OBJECT

      public:
        MeasureCountFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        MeasureCountFilter(const MeasureCountFilter & other);
        virtual ~MeasureCountFilter();
        
        bool evaluate(const ControlCubeGraphNode *) const;
        bool evaluate(const ControlPoint *) const;
        bool evaluate(const ControlMeasure *) const;

        AbstractFilter * clone() const;

        QString getImageDescription() const;
        QString getPointDescription() const;


      private:
        void createWidget();
        void init();
        
        
      private slots:
        void updateMinMax(int);
        void updateMeasureCount(int);


      private:
        QButtonGroup * minMaxGroup;
        QSpinBox * countSpinBox;
        int count;
        bool minimum;
    };
  }
}

#endif
