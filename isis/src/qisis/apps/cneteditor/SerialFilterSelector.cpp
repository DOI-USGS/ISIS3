#include "IsisDebug.h"

#include "SerialFilterSelector.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"
#include "ChooserNameFilter.h"
#include "MeasureIgnoredFilter.h"
#include "PointEditLockedFilter.h"
#include "PointIgnoredFilter.h"
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
  
    selector->addItem("Cube Serial Number");
    selector->insertSeparator(selector->count());
    selector->addItem("Chooser Name");
    selector->addItem("Edit Locked Points");
    selector->addItem("Ignored Points");
    selector->addItem("Point Id");
    selector->insertSeparator(selector->count());
    selector->addItem("Ignored Measures");
  }


  void SerialFilterSelector::changeFilter(int index)
  {
    AbstractFilterSelector::changeFilter(index);
    
    if (index != 0)
    {
      switch (index)
      {
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
      }
      
      connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      mainLayout->insertWidget(2, filter);
    }
    
    emit sizeChanged();
    emit filterChanged();
  }
}
