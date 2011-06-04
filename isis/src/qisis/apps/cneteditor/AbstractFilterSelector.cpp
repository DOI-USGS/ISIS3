#include "IsisDebug.h"

#include "AbstractFilterSelector.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"
#include "PointIdFilter.h"


using std::cerr;

namespace Isis
{
  AbstractFilterSelector::AbstractFilterSelector()
  {
  }
  
  
  AbstractFilterSelector::~AbstractFilterSelector()
  {
    if (selector)
      selector->setCurrentIndex(0);
  }
  
  
  bool AbstractFilterSelector::hasFilter() const
  {
    return filter != NULL;
  }
  
  
  bool AbstractFilterSelector::hasFilter(
      bool (AbstractFilter::*meth)() const) const
  {
    return filter && (filter->*meth)();
  }
  
  
  QString AbstractFilterSelector::getDescription(
      QString (AbstractFilter::*meth)() const) const
  {
    QString description;
    if (filter)
      description = (filter->*meth)();
    
    return description;
  }
  
  
  void AbstractFilterSelector::nullify()
  {
    closeButton = NULL;
    filter = NULL;
    mainLayout = NULL;
    selector = NULL;
  }
  
  
  void AbstractFilterSelector::createSelector()
  {
    closeButton = new QPushButton;
    closeButton->setIcon(QIcon(":close"));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(sendClose()));

    selector = new QComboBox;
    selector->addItem("select");
    connect(selector, SIGNAL(currentIndexChanged(int)),
        this, SLOT(changeFilter(int)));

    mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(closeButton);
    mainLayout->addWidget(selector);
    mainLayout->addStretch();
    mainLayout->setAlignment(closeButton, Qt::AlignTop);
    mainLayout->setAlignment(selector, Qt::AlignTop);
    

    setLayout(mainLayout);
  }
  
  
  void AbstractFilterSelector::changeFilter(int index)
  {
    if (filter)
    {
      QWidget * widget = mainLayout->takeAt(2)->widget();
      ASSERT(widget && widget == filter);
      delete widget;
      filter = NULL;
    }
  }
  
  
  void AbstractFilterSelector::sendClose()
  {
    emit close(this);
  }
}
