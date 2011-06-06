#include "IsisDebug.h"

#include "ResidualMagnitudeFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


using std::cerr;


namespace Isis
{
  ResidualMagnitudeFilter::ResidualMagnitudeFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForImageSuccess) :
      AbstractNumberFilter(flag, parent, minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  ResidualMagnitudeFilter::~ResidualMagnitudeFilter()
  {
  }
  
  
  bool ResidualMagnitudeFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    return evaluateImageFromMeasureFilter(node);
  }
  
  
  bool ResidualMagnitudeFilter::evaluate(const ControlPoint * point) const
  {
    return evaluatePointFromMeasureFilter(point);
  }
  
  
  bool ResidualMagnitudeFilter::evaluate(const ControlMeasure * measure) const
  {
    return AbstractNumberFilter::evaluate(measure->GetResidualMagnitude());
  }
  
  
  QString ResidualMagnitudeFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that has a residual magnitude which is ";
    else
      description += "measures that have residual magnitudes which are ";
    
    description += descriptionSuffix();
    return description;
  }
  
  
  QString ResidualMagnitudeFilter::getPointDescription() const
  {
    return getImageDescription();
  }
  
  
  QString ResidualMagnitudeFilter::getMeasureDescription() const
  {
    return "that have residual magnitudes which are " + descriptionSuffix();
  }
}
