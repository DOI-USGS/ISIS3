#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractPointFilter.h"


class QLineEdit;


namespace Isis
{
  class ControlPoint;

  class PointIdFilter : public AbstractPointFilter
  {
      Q_OBJECT

    public:
      PointIdFilter();
      virtual ~PointIdFilter();

      bool evaluate(ControlPoint const *) const;


    protected:
      void nullify();
      void createWidget();


    private:
      QLineEdit * lineEdit;
  };
}

#endif
