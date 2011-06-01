#ifndef IgnoredFilter_H
#define IgnoredFilter_H

#include "AbstractPointMeasureFilter.h"


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  class IgnoredFilter : public AbstractPointMeasureFilter
  {
      Q_OBJECT

    public:
      IgnoredFilter(int minimumForImageSuccess = -1);
      virtual ~IgnoredFilter();
      
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
