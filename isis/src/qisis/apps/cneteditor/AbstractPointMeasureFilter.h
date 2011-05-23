#ifndef AbstractPointMeasureFilter_H
#define AbstractPointMeasureFilter_H


#include "AbstractFilter.h"


namespace Isis
{
  class ControlPoint;

  class AbstractPointMeasureFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      AbstractPointMeasureFilter(int minimumForImageSuccess = -1);
      virtual ~AbstractPointMeasureFilter();
      
      virtual bool canFilterImages() const;
      virtual bool canFilterPoints() const;
      virtual bool canFilterMeasures() const;
      
      virtual bool evaluate(const ControlCubeGraphNode *) const;
      virtual bool evaluate(const ControlPoint *) const = 0;
      virtual bool evaluate(const ControlMeasure *) const = 0;
      

    public:
      enum Effectiveness
      {
        PointsOnly = 0,
        MeasuresOnly = 1,
        Both = 2
      };
      

    protected:
      virtual void createWidget();
      

    private slots:
      void changeEffectiveness(int);
      
      
    protected:
      Effectiveness effectiveness;
  };
}

#endif
