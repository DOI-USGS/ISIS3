#include "IsisDebug.h"

#include "FilterWidget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "FilterGroup.h"



namespace Isis
{
  FilterWidget::FilterWidget()
  {
    nullify();
    
    filterGroups = new QList< FilterGroup * >();
    
    addGroupButton = new QPushButton("Add Filter Group");
    connect(addGroupButton, SIGNAL(clicked()), this, SLOT(addGroup()));

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(addGroupButton);
    mainLayout->addStretch();
    
    setLayout(mainLayout);
  }


  FilterWidget::~FilterWidget()
  {
    if (filterGroups)
    {
      delete filterGroups;
      filterGroups = NULL;
    }
  }
  
  
  void FilterWidget::nullify()
  {
    addGroupButton = NULL;
    mainLayout = NULL;
    filterGroups = NULL;
  }
  
  
  void FilterWidget::addGroup()
  {
    FilterGroup * newGroup = new FilterGroup();
    connect(newGroup, SIGNAL(close(FilterGroup *)),
            this, SLOT(delGroup(FilterGroup *)));
    mainLayout->insertWidget(mainLayout->count() - 1, newGroup);
    filterGroups->append(newGroup);
  }
  
  
  void FilterWidget::delGroup(FilterGroup * filterGroup)
  {
    mainLayout->removeWidget(filterGroup);
    delete filterGroup;
    filterGroups->removeOne(filterGroup);
  }
}
