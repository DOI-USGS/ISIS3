#include "IsisDebug.h"

#include "AbstractFilterSelector.h"

#include <algorithm>
#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QWriteLocker>

#include "AbstractFilter.h"
#include "PointIdFilter.h"


using std::nothrow;
using std::swap;


namespace Isis
{
  namespace CnetViz
  {
    AbstractFilterSelector::AbstractFilterSelector()
    {
      nullify();
    }


    AbstractFilterSelector::~AbstractFilterSelector()
    {
      deleteFilter();
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
      QString(AbstractFilter::*meth)() const) const
    {
      QString description;
      if (filter)
        description = (filter->*meth)();

      return description;
    }


    AbstractFilterSelector & AbstractFilterSelector::operator=(
      const AbstractFilterSelector & other)
    {
      getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
      if (filter && other.filter)
        setFilter(other.filter->clone());

      return *this;
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
      selector->addItem("---- select ----");
      selector->insertSeparator(selector->count());
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


    QComboBox * AbstractFilterSelector::getSelector() const
    {
      return selector;
    }


    QHBoxLayout * AbstractFilterSelector::getMainLayout() const
    {
      return mainLayout;
    }


    AbstractFilter * AbstractFilterSelector::getFilter() const
    {
      return filter;
    }


    void AbstractFilterSelector::setFilter(AbstractFilter * someFilter)
    {
      if (filter)
      {
        delete filter;
        filter = NULL;
      }

      filter = someFilter;
      connect(getFilter(), SIGNAL(filterChanged()),
          this, SIGNAL(filterChanged()));
      getMainLayout()->insertWidget(2, getFilter());
    }


    void AbstractFilterSelector::deleteFilter()
    {
      if (filter)
      {
  //       QWidget * widget = mainLayout->takeAt(2)->widget();
  //       ASSERT(widget && widget == filter);
        delete filter;
        filter = NULL;
      }
    }


    void AbstractFilterSelector::sendClose()
    {
      emit close(this);
    }
  }
}
