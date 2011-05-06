#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractFilter.h"


class QLineEdit;


namespace Isis
{
  class ControlPoint;

  class PointIdFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      PointIdFilter();
      virtual ~PointIdFilter();

      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;
      bool evaluate(const ControlCubeGraphNode *) const;


    protected:
      void nullify();
      void createWidget();


    private:
      QLineEdit * lineEdit;
  };
}

#endif
