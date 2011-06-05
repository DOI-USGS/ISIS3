#include "IsisDebug.h"

#include "PointMeasureFilterSelector.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"
#include "CubeSerialNumberFilter.h"
#include "ChooserNameFilter.h"
#include "PointEditLockedFilter.h"
#include "MeasureIgnoredFilter.h"
#include "PointIgnoredFilter.h"
#include "PointIdFilter.h"


using std::cerr;

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
              AbstractFilter::Measures, this, 1); //FIXME
          break;
        case 8:
          filter = new MeasureIgnoredFilter(AbstractFilter::Points |
              AbstractFilter::Measures, this, 1); //FIXME
          break;
      }
      
      connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      mainLayout->insertWidget(2, filter);
    }
    
    emit sizeChanged();
    emit filterChanged();
  }
}
