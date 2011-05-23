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
  
  
  QString IgnoredFilter::getDescription() const
  {
    QString description = "are ";
     
    if (!inclusive())
      description += "not ";
    
    description += "ignored";
    
    return description;
  }
}
