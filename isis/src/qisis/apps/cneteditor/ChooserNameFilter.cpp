#include "IsisDebug.h"

#include "ChooserNameFilter.h"

#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iString.h"


namespace Isis
{
  ChooserNameFilter::ChooserNameFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractStringFilter(flag, parent, minimumForSuccess)
  {
    nullify();
    createWidget();
  }


  ChooserNameFilter::~ChooserNameFilter()
  {
  }


  bool ChooserNameFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    return evaluateImageFromPointFilter(node);
  }


  bool ChooserNameFilter::evaluate(const ControlPoint * point) const
  {
    return AbstractStringFilter::evaluate((QString) point->GetChooserName());
  }
  
  
  bool ChooserNameFilter::evaluate(const ControlMeasure *) const
  {
    return true;
  }
  
  
  QString ChooserNameFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    
    if (getMinForSuccess() == 1)
      description += "point with it's chooser name ";
    else
      description += "points with chooser names ";
    
    description += descriptionSuffix();
    return description;
  }
  
  
  QString ChooserNameFilter::getPointDescription() const
  {
    return "have chooser names " + descriptionSuffix();
  }
}
