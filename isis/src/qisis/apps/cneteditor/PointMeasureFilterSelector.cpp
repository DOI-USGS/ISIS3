#include "IsisDebug.h"

#include "PointMeasureFilterSelector.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"
// #include "ChooserNameFilter.h"
// #include "EditLockFilter.h"
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
  }


  void PointMeasureFilterSelector::changeFilter(int index)
  {
    AbstractFilterSelector::changeFilter(index);
    
    if (index != 0)
    {
      switch (index)
      {
//         case 1:
//           filter = new ChooserNameFilter(AbstractFilter::Points);
//           break;
//         case 2:
//           filter = new EditLockFilter(AbstractFilter::Points |
//                                       AbstractFilter::Measures);
//           break;
        case 3:
          filter = new PointIgnoredFilter(AbstractFilter::Points);
          break;
        case 4:
          filter = new PointIdFilter(AbstractFilter::Points);
          break;
      }
      
      connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      mainLayout->insertWidget(2, filter);
    }
    
    emit sizeChanged();
    emit filterChanged();
  }
}
