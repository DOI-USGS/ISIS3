#include "IsisDebug.h"

#include "SerialFilterSelector.h"

#include <QComboBox>
#include <QHBoxLayout>

#include "AbstractFilter.h"
#include "ChooserNameFilter.h"
#include "CubeSerialNumberFilter.h"
#include "LineResidualFilter.h"
#include "MeasureIgnoredFilter.h"
#include "PointEditLockedFilter.h"
#include "PointIgnoredFilter.h"
#include "PointIdFilter.h"
#include "ResidualMagnitudeFilter.h"
#include "SampleResidualFilter.h"



namespace Isis
{
  SerialFilterSelector::SerialFilterSelector()
  {
    nullify();
    createSelector();
  }


  SerialFilterSelector::~SerialFilterSelector()
  {
  }
 
 
  void SerialFilterSelector::createSelector()
  {
    AbstractFilterSelector::createSelector();
  
    selector->addItem("Cube Serial Number");
    selector->insertSeparator(selector->count());
    selector->addItem("Chooser Name");
    selector->addItem("Edit Locked Points");
    selector->addItem("Ignored Points");
    selector->addItem("Point Id");
    selector->insertSeparator(selector->count());
    selector->addItem("Ignored Measures");
    selector->addItem("Line Residual");
    selector->addItem("Residual Magnitude");
    selector->addItem("Sample Residual");
  }


  void SerialFilterSelector::changeFilter(int index)
  {
    AbstractFilterSelector::changeFilter(index);
    
    if (index != 0)
    {
      switch (index)
      {
        case 2:
          filter = new CubeSerialNumberFilter(AbstractFilter::Images, this);
          break;
        case 4:
          filter = new ChooserNameFilter(AbstractFilter::Images |
              AbstractFilter::Points, this, 1);
          break;
        case 5:
          filter = new PointEditLockedFilter(AbstractFilter::Images |
              AbstractFilter::Points, this, 1);
          break;
        case 6:
          filter = new PointIgnoredFilter(AbstractFilter::Images |
              AbstractFilter::Points, this, 1);
          break;
        case 7:
          filter = new PointIdFilter(AbstractFilter::Images |
              AbstractFilter::Points, this, 1);
          break;
        case 9:
          filter = new MeasureIgnoredFilter(AbstractFilter::Images, this, 1);
          break;
        case 10:
          filter = new LineResidualFilter(AbstractFilter::Images, this, 1);
          break;
        case 11:
          filter = new ResidualMagnitudeFilter(AbstractFilter::Images, this, 1);
          break;
        case 12:
          filter = new SampleResidualFilter(AbstractFilter::Images, this, 1);
          break;
      }
      
      connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      mainLayout->insertWidget(2, filter);
    }
    
    emit sizeChanged();
    emit filterChanged();
  }
}
