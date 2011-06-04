#include "IsisDebug.h"

#include "PointIgnoredFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  PointIgnoredFilter::PointIgnoredFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      int minimumForImageSuccess) : AbstractFilter(flag, minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  PointIgnoredFilter::~PointIgnoredFilter()
  {
  }
  
  
  bool PointIgnoredFilter::evaluate(const ControlPoint * point) const
  {
    return AbstractFilter::evaluate(point, &ControlPoint::IsIgnored);
  }
  
  
  QString PointIgnoredFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForImageSuccess() == 1)
      description += "point that is ";
    else
      description += "points that are ";

    if (inclusive())
      description += "ignored";
    else
      description += "not ignored";
    
    return description;
  }
  
  
  QString PointIgnoredFilter::getPointDescription() const
  {
    QString description = "are ";
    
    if (inclusive())
      description += "ignored";
    else
      description += "not ignored";
    
    return description;
  }
}
