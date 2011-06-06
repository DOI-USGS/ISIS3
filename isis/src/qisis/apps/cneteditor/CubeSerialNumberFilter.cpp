#include "IsisDebug.h"

#include "CubeSerialNumberFilter.h"

#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iString.h"


namespace Isis
{
  CubeSerialNumberFilter::CubeSerialNumberFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractStringFilter(flag, parent, minimumForSuccess)
  {
    nullify();
    createWidget();
  }


  CubeSerialNumberFilter::~CubeSerialNumberFilter()
  {
  }


  bool CubeSerialNumberFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    return AbstractStringFilter::evaluate((QString) node->getSerialNumber());
  }
  
  
  bool CubeSerialNumberFilter::evaluate(const ControlPoint * point) const
  {
    return evaluatePointFromMeasureFilter(point);
  }
  
  
  bool CubeSerialNumberFilter::evaluate(const ControlMeasure * measure) const
  {
    return AbstractStringFilter::evaluate(
        (QString) measure->GetCubeSerialNumber());
  }
  
  
  QString CubeSerialNumberFilter::getImageDescription() const
  {
    return getMeasureDescription();
  }
  
  
  QString CubeSerialNumberFilter::getPointDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    
    if (getMinForSuccess() == 1)
      description += "measure with it's cube serial number ";
    else
      description += "measures with cube serial numbers ";
    
    description += descriptionSuffix();
    return description;
  }
  
  
  QString CubeSerialNumberFilter::getMeasureDescription() const
  {
    return "have cube serial numbers " + descriptionSuffix();
  }
}
