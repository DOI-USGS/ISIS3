#include "IsisDebug.h"

#include "PointFilterSelector.h"

#include <QComboBox>
#include <QHBoxLayout>

#include "AbstractFilter.h"
#include "PointIdFilter.h"


namespace Isis
{
  PointFilterSelector::PointFilterSelector()
  {
    selector = new QComboBox;
    selector->addItem("select");
    selector->addItem("Points by Point Id");
    
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(selector);
    setLayout(mainLayout);
  }
  
  
  PointFilterSelector::~PointFilterSelector()
  {
    if (selector)
      selector->setCurrentIndex(0);
  }
  
  
  void PointFilterSelector::nullify()
  {
    selector = NULL;
  }
  
  
  void PointFilterSelector::filterChanged(int index)
  {
    if (filter)
    {
      QLayoutItem * item = mainLayout->takeAt(1);
      ASSERT(item);
      delete item;
      filter = NULL;
    }
    
    switch (index)
    {
      case 1:
        mainLayout->addWidget(new PointIdFilter);
    }
  }
  
  
  AbstractFilter * PointFilterSelector::getFilter()
  {
    return filter;
  }
}
