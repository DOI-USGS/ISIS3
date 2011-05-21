#include "IsisDebug.h"

#include "ConnectionFilterSelector.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"


using std::cerr;

namespace Isis
{
  ConnectionFilterSelector::ConnectionFilterSelector()
  {
    nullify();
    createSelector();
  }


  ConnectionFilterSelector::~ConnectionFilterSelector()
  {
  }
 
 
  void ConnectionFilterSelector::createSelector()
  {
    AbstractFilterSelector::createSelector();
  
//     selector->addItem("Point Id");
  }


  void ConnectionFilterSelector::changeFilter(int index)
  {
    AbstractFilterSelector::changeFilter(index);
    
    if (index != 0)
    {
//       switch (index)
//       {
//         case 1:
//           filter = new PointIdFilter;
//           break;
//         case 2:
//           filter = new PointIdFilter;
//           break;
//       }
//       
//       connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
//       mainLayout->insertWidget(2, filter);
    }
  }
}
