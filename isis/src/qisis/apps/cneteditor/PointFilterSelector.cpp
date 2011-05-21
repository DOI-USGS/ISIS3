#include "IsisDebug.h"

#include "PointFilterSelector.h"

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
  PointFilterSelector::PointFilterSelector()
  {
    nullify();
    createSelector();
  }


  PointFilterSelector::~PointFilterSelector()
  {
  }
 
 
  void PointFilterSelector::createSelector()
  {
    AbstractFilterSelector::createSelector();
  
    selector->addItem("Chooser Name");
    selector->addItem("Ignored Points");
    selector->addItem("Point Id");
  }


  void PointFilterSelector::changeFilter(int index)
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
