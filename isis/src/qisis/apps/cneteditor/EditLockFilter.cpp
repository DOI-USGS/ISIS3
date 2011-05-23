#include "IsisDebug.h"

#include "EditLockFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  EditLockFilter::EditLockFilter(int minimumForImageSuccess) :
      AbstractPointMeasureFilter(minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  EditLockFilter::~EditLockFilter()
  {
  }


  bool EditLockFilter::evaluate(const ControlPoint * point) const
  {
    return AbstractFilter::evaluate(point, &ControlPoint::IsEditLocked);
  }


  bool EditLockFilter::evaluate(const ControlMeasure * measure) const
  {
    return AbstractFilter::evaluate(measure, &ControlMeasure::IsEditLocked);
  }

  
  QString EditLockFilter::getDescription() const
  {
    QString description = "are ";
     
    if (!inclusive())
      description += "not ";
    
    description += "edit locked";
    
    return description;
  }
}
