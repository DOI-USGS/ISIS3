#include "IsisDebug.h"

#include "IgnoredFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  IgnoredFilter::IgnoredFilter(int minimumForImageSuccess) :
      AbstractPointMeasureFilter(minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  IgnoredFilter::~IgnoredFilter()
  {
  }
  
  
  bool IgnoredFilter::evaluate(const ControlPoint * point) const
  {
    return AbstractFilter::evaluate(point, &ControlPoint::IsIgnored);
  }
  
  
  bool IgnoredFilter::evaluate(const ControlMeasure * measure) const
  {
    return AbstractFilter::evaluate(measure, &ControlMeasure::IsIgnored);
  }
  
  
  QString IgnoredFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    if (effectiveness == AbstractPointMeasureFilter::Both)
    {
      if (getMinForImageSuccess() == 1)
        description += "point or measure that is ";
      else
        description += "points or measures that are ";
    }
    else
    {
      if (effectiveness == AbstractPointMeasureFilter::PointsOnly)
        description += "point";
      else
        description += "measure";
        
      if (getMinForImageSuccess() == 1)
        description += " that is ";
      else
        description += "s that are ";
    }
    
    if (inclusive())
      description += "ignored";
    else
      description += "not ignored";
    
    return description;
  }
  
  
  QString IgnoredFilter::getPointDescription() const
  {
    QString description = "are ";
    
    if (inclusive())
      description += "ignored";
    else
      description += "not ignored";
    
    return description;
  }

  
  QString IgnoredFilter::getMeasureDescription() const
  {
    return getPointDescription();
  }
}
