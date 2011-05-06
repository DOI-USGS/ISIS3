#include "IsisDebug.h"

#include "FilterGroup.h"

#include <QPushButton>
#include <QHBoxLayout>



namespace Isis
{
  FilterGroup::FilterGroup()
  {
    nullify();
    
    closeButton = new QPushButton("X: " + QString::number((long)this));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(sendClose()));
    
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(closeButton);
    setLayout(mainLayout);
  }


  FilterGroup::~FilterGroup()
  {
  }
  
  
  void FilterGroup::nullify()
  {
    closeButton = NULL;
  }
  
  
  void FilterGroup::sendClose()
  {
    emit close(this);
  }
}
