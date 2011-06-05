#include "IsisDebug.h"

#include "PointEditLockedFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  PointEditLockedFilter::PointEditLockedFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForImageSuccess) :
      AbstractFilter(flag, parent, minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  PointEditLockedFilter::~PointEditLockedFilter()
  {
  }
  
  
  bool PointEditLockedFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    return AbstractFilter::evaluateImageFromPointFilter(node);
  }


  bool PointEditLockedFilter::evaluate(const ControlPoint * point) const
  {
    return AbstractFilter::evaluate(point, &ControlPoint::IsEditLocked);
  }

  
  QString PointEditLockedFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    
    if (getMinForSuccess() == 1)
      description += "point that is ";
    else
      description += "points that are ";

    if (inclusive())
      description += "edit locked";
    else
      description += "not edit locked";
    
    return description;
  }
  
  
  QString PointEditLockedFilter::getPointDescription() const
  {
    QString description = "are ";
    
    if (inclusive())
      description += "edit locked";
    else
      description += "not edit locked";
    
    return description;
  }
}
