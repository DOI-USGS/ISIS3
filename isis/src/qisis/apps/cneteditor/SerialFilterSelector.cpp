#include "IsisDebug.h"

#include "SerialFilterSelector.h"

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
  
    selector->addItem("Chooser Name");
    selector->addItem("Ignored");
    selector->addItem("Point Id");
  }


  void SerialFilterSelector::changeFilter(int index)
  {
    AbstractFilterSelector::changeFilter(index);
    
    if (index != 0)
    {
      switch (index)
      {
        case 1:
          filter = new ChooserNameFilter(1);
          break;
        case 2:
          filter = new IgnoredFilter(1);
          break;
        case 3:
          filter = new PointIdFilter(1);
          break;
      }
      
      connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      mainLayout->insertWidget(2, filter);
    }
  }
}
