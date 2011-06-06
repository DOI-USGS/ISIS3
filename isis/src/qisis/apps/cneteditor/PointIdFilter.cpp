#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlPoint.h"
#include "iString.h"


namespace Isis
{
  PointIdFilter::PointIdFilter(AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractStringFilter(flag, parent, minimumForSuccess)
  {
    nullify();
    createWidget();
  }


  PointIdFilter::~PointIdFilter()
  {
  }
  
  
  bool PointIdFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    return evaluateImageFromPointFilter(node);
  }
  
  
  bool PointIdFilter::evaluate(const ControlPoint * point) const
  {
    return AbstractStringFilter::evaluate((QString) point->GetId());
  }
  
  
  bool PointIdFilter::evaluate(const ControlMeasure *) const
  {
    return true;
  }
  
  
  QString PointIdFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    
    if (getMinForSuccess() == 1)
      description += "point with it's ID ";
    else
      description += "points with IDs ";
    
    description += descriptionSuffix();
    return description;
  }
  
  
  QString PointIdFilter::getPointDescription() const
  {
    return "have IDs " + descriptionSuffix();
  }
}
