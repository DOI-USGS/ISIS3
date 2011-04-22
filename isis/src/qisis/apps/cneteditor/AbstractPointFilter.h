#ifndef AbstractPointFilter_H
#define AbstractPointFilter_H

#include "AbstractFilter.h"


namespace Isis
{
  class ControlPoint;

  class AbstractPointFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      AbstractPointFilter();
      virtual ~AbstractPointFilter();

      virtual bool evaluate(ControlPoint const *) const = 0;
  };
}

#endif
