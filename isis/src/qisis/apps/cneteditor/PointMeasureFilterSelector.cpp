#include "IsisDebug.h"

#include "PointMeasureFilterSelector.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"
#include "ChooserNameFilter.h"
#include "IgnoredFilter.h"
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
    selector->addItem("Ignored");
    selector->addItem("Point Id");
  }


  void PointMeasureFilterSelector::changeFilter(int index)
  {
    AbstractFilterSelector::changeFilter(index);
    
    if (index != 0)
    {
      switch (index)
      {
        case 1:
          filter = new ChooserNameFilter;
          break;
        case 2:
          filter = new IgnoredFilter;
          break;
        case 3:
          filter = new PointIdFilter;
          break;
      }
      
      connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      mainLayout->insertWidget(2, filter);
    }
    
    emit filterChanged();
  }
}
