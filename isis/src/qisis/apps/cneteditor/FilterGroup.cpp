#include "IsisDebug.h"

#include "FilterGroup.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>

#include "iException.h"
#include "iString.h"

#include "AbstractFilterSelector.h"
#include "ConnectionFilterSelector.h"
#include "PointMeasureFilterSelector.h"
#include "SerialFilterSelector.h"


using std::nothrow;
using std::swap;

namespace Isis
{
  FilterGroup::FilterGroup(QString type)
  {
    nullify();

    filterType = new QString(type);

    init();
    addSelector();
  }


  FilterGroup::FilterGroup(const FilterGroup & other)
  {
    nullify();

    filterType = new QString(*other.filterType);

    init();

    foreach(AbstractFilterSelector * selector, *other.selectors)
    {
      AbstractFilterSelector * newSelector = NULL;
      if (*filterType == "Points and Measures")
      {
        newSelector = new PointMeasureFilterSelector(
          *((PointMeasureFilterSelector *) selector));
      }
      else
      {
        if (*filterType == "Images and Points")
        {
          newSelector = new SerialFilterSelector(
            *((SerialFilterSelector *) selector));
        }
        else
        {
          if (*filterType == "Connections")
          {
            newSelector = new ConnectionFilterSelector(
              *((ConnectionFilterSelector *) selector));
          }
        }
      }

      addSelector(newSelector);
    }

    buttonGroup->button(other.buttonGroup->checkedId())->click();
  }


  FilterGroup::~FilterGroup()
  {
    if (buttonGroup)
    {
      delete buttonGroup;
      buttonGroup = NULL;
    }

    if (selectors)
    {
      delete selectors;
      selectors = NULL;
    }

    if (filterType)
    {
      delete filterType;
      filterType = NULL;
    }
  }


  bool FilterGroup::hasFilter(bool (AbstractFilter::*meth)() const) const
  {
    bool found = false;

    for (int i = 0; !found && i < selectors->size(); i++)
      if (meth)
        found = selectors->at(i)->hasFilter(meth);
      else
        found = selectors->at(i)->hasFilter();

    return found;
  }


  void FilterGroup::nullify()
  {
    buttonGroup = NULL;
    closeButton = NULL;
    groupBoxLayout = NULL;
    selectors = NULL;
  }


  void FilterGroup::init()
  {
    selectors = new QList< AbstractFilterSelector * >;

    QLabel * descriptionLabel = new QLabel("Combine filters using logic "
        "type: ");
    QFont descriptionFont("SansSerif", 11);
    descriptionLabel->setFont(descriptionFont);
    QFont logicTypeFont("SansSerif", 12, QFont::Bold);
    QRadioButton * andButton = new QRadioButton("and");
    andButton->setFont(logicTypeFont);
    QRadioButton * orButton = new QRadioButton("or");
    orButton->setFont(logicTypeFont);
    buttonGroup = new QButtonGroup;
    buttonGroup->addButton(andButton, 0);
    buttonGroup->addButton(orButton, 1);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this,
        SLOT(changeFilterCombinationLogic(int)));

    // FIXME: this should be controlled by QSettings
    andButton->click();

    QHBoxLayout * logicLayout = new QHBoxLayout;
    QMargins margins = logicLayout->contentsMargins();
    margins.setBottom(4);
    logicLayout->setContentsMargins(margins);
    logicLayout->addStretch();
    logicLayout->addWidget(descriptionLabel);
    logicLayout->addWidget(andButton);
    logicLayout->addWidget(orButton);
    logicLayout->addStretch();
    logicWidget = new QWidget;
    logicWidget->setLayout(logicLayout);

    newSelectorButton = new QPushButton;
    newSelectorButton->setIcon(QIcon(":add"));
    connect(newSelectorButton, SIGNAL(clicked()), this, SLOT(addSelector()));
    QHBoxLayout * newSelectorLayout = new QHBoxLayout;
    newSelectorLayout->addWidget(newSelectorButton);
    newSelectorLayout->addStretch();

    groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addWidget(logicWidget);
    groupBoxLayout->addLayout(newSelectorLayout);
    QGroupBox * groupBox = new QGroupBox;
    groupBox->setLayout(groupBoxLayout);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    margins = mainLayout->contentsMargins();
    margins.setTop(2);
    margins.setBottom(2);
    mainLayout->setContentsMargins(margins);
    mainLayout->addWidget(groupBox);

    setLayout(mainLayout);
  }


  void FilterGroup::addSelector()
  {
    AbstractFilterSelector * filterSelector = NULL;
    if (*filterType == "Points and Measures")
      filterSelector = new PointMeasureFilterSelector;
    else
      if (*filterType == "Images and Points")
        filterSelector = new SerialFilterSelector;
      else
        if (*filterType == "Connections")
          filterSelector = new ConnectionFilterSelector;

    addSelector(filterSelector);
  }


  void FilterGroup::addSelector(AbstractFilterSelector * newSelector)
  {
    ASSERT(newSelector);

    if (newSelector)
    {
      connect(newSelector, SIGNAL(close(AbstractFilterSelector *)),
          this, SLOT(deleteSelector(AbstractFilterSelector *)));
      connect(newSelector, SIGNAL(filterChanged()),
          this, SIGNAL(filterChanged()));
      connect(newSelector, SIGNAL(sizeChanged()),
          this, SLOT(sendSizeChanged()));
      groupBoxLayout->insertWidget(groupBoxLayout->count() - 1, newSelector);
      selectors->append(newSelector);
      selectors->size() > 1 ? logicWidget->show() : logicWidget->hide();
    }

    sendSizeChanged();
  }


  void FilterGroup::deleteSelector(AbstractFilterSelector * filterSelector)
  {
    groupBoxLayout->removeWidget(filterSelector);
    delete filterSelector;
    selectors->removeOne(filterSelector);

    int selectorsSize = selectors->size();
    selectorsSize > 1 ? logicWidget->show() : logicWidget->hide();
    if (!selectorsSize)
      emit close(this);
    else
      emit filterChanged();
  }


  void FilterGroup::sendClose()
  {
    emit close(this);
  }


  void FilterGroup::sendSizeChanged()
  {
    emit sizeChanged(this);
  }


  void FilterGroup::changeFilterCombinationLogic(int button)
  {
    andFiltersTogether = button == 0;
    emit filterChanged();
  }


  bool FilterGroup::filtersAreAndedTogether() const
  {
    return andFiltersTogether;
  }


  FilterGroup & FilterGroup::operator=(FilterGroup other)
  {
    ASSERT(filterType);
    ASSERT(selectors);
    ASSERT(buttonGroup);

    // create temporary list of new selectors
    QList< AbstractFilterSelector * > newSelectors;
    foreach(AbstractFilterSelector * selector, *other.selectors)
    {
      AbstractFilterSelector * newSelector = NULL;
      if (*filterType == "Points and Measures")
      {
        newSelector = new(nothrow) PointMeasureFilterSelector(
          *((PointMeasureFilterSelector *) selector));
      }
      else
      {
        if (*filterType == "Images and Points")
        {
          newSelector = new(nothrow) SerialFilterSelector(
            *((SerialFilterSelector *) selector));
        }
        else
        {
          if (*filterType == "Connections")
          {
            newSelector = new(nothrow) ConnectionFilterSelector(
              *((ConnectionFilterSelector *) selector));
          }
        }
      }

      if (newSelector)
      {
        *newSelector = *selector;
        newSelectors.append(newSelector);
      }
    }

    // if all is ok, and it is safe to assign
    if (newSelectors.size() == other.selectors->size())
    {
      foreach(AbstractFilterSelector * selector, *selectors)
      {
        deleteSelector(selector);
      }

      foreach(AbstractFilterSelector * newSelector, newSelectors)
      {
        addSelector(newSelector);
      }

      swap(*filterType, *other.filterType);
      buttonGroup->button(other.buttonGroup->checkedId())->click();
    }
    else
    {
      // clean up any temp groups
      foreach(AbstractFilterSelector * newSelector, newSelectors)
      {
        if (newSelector)
        {
          delete newSelector;
          newSelector = NULL;
        }
      }

      iString msg = "Assignment of FilterGroup failed";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return *this;
  }


  QString FilterGroup::getDescription(
    bool (AbstractFilter::*hasFilterMeth)() const,
    QString(AbstractFilter::*descriptionMeth)() const) const
  {
    QString description;

    ASSERT(selectors);

    QList< AbstractFilterSelector * > selectorsWithFilters;
    for (int i = 0; i < selectors->size(); i++)
      if (selectors->at(i)->hasFilter(hasFilterMeth))
        selectorsWithFilters.append(selectors->at(i));

    int numFilters = selectorsWithFilters.size();

    if (numFilters)
    {
      QString logic = "<b> ";
      if (andFiltersTogether)
        logic += "and";
      else
        logic += "or";
      logic += " </b>";

      for (int i = 0; i < numFilters - 1; i++)
      {
        description += selectorsWithFilters[i]->getDescription(descriptionMeth)
            + logic;
      }

      description += selectorsWithFilters[numFilters - 1]->getDescription(
          descriptionMeth);
    }

    return description;
  }
}
