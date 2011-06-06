#include "IsisDebug.h"

#include "PointMeasureFilterSelector.h"

#include <QComboBox>
#include <QHBoxLayout>

#include "AbstractFilter.h"
#include "CubeSerialNumberFilter.h"
#include "ChooserNameFilter.h"
#include "LineResidualFilter.h"
#include "PointEditLockedFilter.h"
#include "MeasureIgnoredFilter.h"
#include "PointIgnoredFilter.h"
#include "PointIdFilter.h"
#include "ResidualMagnitudeFilter.h"
#include "SampleResidualFilter.h"


namespace Isis
{
  PointMeasureFilterSelector::PointMeasureFilterSelector()
  {
    nullify();
    createSelector();
  }


  PointMeasureFilterSelector::~PointMeasureFilterSelector()
  {
  }
 
 
  void PointMeasureFilterSelector::createSelector()
  {
    AbstractFilterSelector::createSelector();
  
    selector->addItem("Chooser Name");
    selector->addItem("Edit Locked Points");
    selector->addItem("Ignored Points");
    selector->addItem("Point Id");
    selector->insertSeparator(selector->count());
    selector->addItem("Cube Serial Number");
    selector->addItem("Ignored Measures");
    selector->addItem("Line Residual");
    selector->addItem("Residual Magnitude");
    selector->addItem("Sample Residual");
  }


  void PointMeasureFilterSelector::changeFilter(int index)
  {
    AbstractFilterSelector::changeFilter(index);
    
    if (index != 0)
    {
      switch (index)
      {
        case 2:
          filter = new ChooserNameFilter(AbstractFilter::Points, this);
          break;
        case 3:
          filter = new PointEditLockedFilter(AbstractFilter::Points, this);
          break;
        case 4:
          filter = new PointIgnoredFilter(AbstractFilter::Points, this);
          break;
        case 5:
          filter = new PointIdFilter(AbstractFilter::Points, this);
          break;
        case 7:
          filter = new CubeSerialNumberFilter(AbstractFilter::Points |
              AbstractFilter::Measures, this, 1);
          break;
        case 8:
          filter = new MeasureIgnoredFilter(AbstractFilter::Points |
              AbstractFilter::Measures, this, 1);
          break;
        case 9:
          filter = new LineResidualFilter(AbstractFilter::Points |
              AbstractFilter::Measures, this, 1);
          break;
        case 10:
          filter = new ResidualMagnitudeFilter(AbstractFilter::Points |
              AbstractFilter::Measures, this, 1);
          break;
        case 11:
          filter = new SampleResidualFilter(AbstractFilter::Points |
              AbstractFilter::Measures, this, 1);
          break;
      }
      
      connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      mainLayout->insertWidget(2, filter);
    }
    
    emit sizeChanged();
    emit filterChanged();
  }
}
