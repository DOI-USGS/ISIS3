#ifndef EditLockFilter_H
#define EditLockFilter_H

#include "AbstractPointMeasureFilter.h"


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  class EditLockFilter : public AbstractPointMeasureFilter
  {
      Q_OBJECT

    public:
      EditLockFilter(int minimumForImageSuccess = -1);
      virtual ~EditLockFilter();

      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      QString getDescription() const;
  };
}

#endif
