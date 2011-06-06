#include "IsisDebug.h"

#include "LineResidualFilter.h"

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


namespace Isis
{
  LineResidualFilter::LineResidualFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractNumberFilter(flag, parent, minimumForSuccess)
  {
    nullify();
    createWidget();
  }


  LineResidualFilter::~LineResidualFilter()
  {
  }
  
  
  bool LineResidualFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    return evaluateImageFromMeasureFilter(node);
  }
  
  
  bool LineResidualFilter::evaluate(const ControlPoint * point) const
  {
    return evaluatePointFromMeasureFilter(point);
  }
  
  
  bool LineResidualFilter::evaluate(const ControlMeasure * measure) const
  {
    return AbstractNumberFilter::evaluate(measure->GetLineResidual());
  }
  
  
  QString LineResidualFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that has a line residual which is ";
    else
      description += "measures that have line residuals which are ";
    
    description += descriptionSuffix();
    return description;
  }
  
  
  QString LineResidualFilter::getPointDescription() const
  {
    return getImageDescription();
  }
  
  
  QString LineResidualFilter::getMeasureDescription() const
  {
    return "that have line residuals which are " + descriptionSuffix();
  }
}
