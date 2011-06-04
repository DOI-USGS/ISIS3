#ifndef PointIgnoredFilter_H
#define PointIgnoredFilter_H

#include "AbstractFilter.h"


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  class PointIgnoredFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      PointIgnoredFilter(AbstractFilter::FilterEffectivenessFlag flag,
                    int minimumForImageSuccess = -1);
      virtual ~PointIgnoredFilter();
      bool evaluate(const ControlPoint *) const;
      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
