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

#include "AbstractFilterSelector.h"
#include "ConnectionFilterSelector.h"
#include "PointFilterSelector.h"
#include "SerialFilterSelector.h"


namespace Isis
{
  FilterGroup::FilterGroup(QString type)
  {
    nullify();

    selectors = new QList< AbstractFilterSelector * >;
    
    filterType = type;

    QLabel * descriptionLabel = new QLabel("Combine filters using logic type: ");
    QFont descriptionFont("SansSerif", 11);
    descriptionLabel->setFont(descriptionFont);
    QFont logicTypeFont("SansSerif", 12, QFont::Bold);
    QRadioButton * andButton = new QRadioButton("and");
    andButton->setFont(logicTypeFont);
    QRadioButton * orButton = new QRadioButton("or");
    orButton->setFont(logicTypeFont);
    QButtonGroup * buttonGroup = new QButtonGroup;
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
    connect(newSelectorButton, SIGNAL(clicked()), this, SLOT(addFilter()));
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
    addFilter();
  }


  FilterGroup::~FilterGroup()
  {
    if (selectors)
    {
      delete selectors;
      selectors = NULL;
    }
  }


  void FilterGroup::nullify()
  {
    closeButton = NULL;
    groupBoxLayout = NULL;
    selectors = NULL;
  }


  void FilterGroup::addFilter()
  {
    AbstractFilterSelector * filterSelector = NULL;
    if (filterType == "Points")
      filterSelector = new PointFilterSelector;
    else
      if (filterType == "Images")
        filterSelector = new SerialFilterSelector;
      else
        if (filterType == "Connections")
          filterSelector = new ConnectionFilterSelector;
      
    if (filterSelector)
    {
      connect(filterSelector, SIGNAL(close(AbstractFilterSelector *)),
          this, SLOT(deleteFilter(AbstractFilterSelector *)));
      connect(filterSelector, SIGNAL(filterChanged()),
          this, SIGNAL(filterChanged()));
      groupBoxLayout->insertWidget(groupBoxLayout->count() - 1, filterSelector);
      selectors->append(filterSelector);
      selectors->size() > 1 ? logicWidget->show() : logicWidget->hide();
    }
  }


  void FilterGroup::deleteFilter(AbstractFilterSelector * filterSelector)
  {
    groupBoxLayout->removeWidget(filterSelector);
    delete filterSelector;
    selectors->removeOne(filterSelector);
    
    int selectorsSize = selectors->size();
    selectorsSize > 1 ? logicWidget->show() : logicWidget->hide();
    if (!selectorsSize)
      emit close(this);
  }


  void FilterGroup::sendClose()
  {
    emit close(this);
  }


  void FilterGroup::changeFilterCombinationLogic(int button)
  {
    andFiltersTogether = button == 0;
    emit filterChanged();
  }
  
  
  bool FilterGroup::hasFilter() const
  {
    bool foundFilter = false;
    
    if (selectors)
      for (int i = 0; !foundFilter && i < selectors->size(); i++)
        foundFilter = selectors->at(i)->hasFilter();
    
    return foundFilter;
  }
  
  
  bool FilterGroup::filtersAreAndedTogether() const
  {
    return andFiltersTogether;
  }
  
  
  QString FilterGroup::getDescription() const
  {
    QString description;
    
    if (hasFilter())
    {
      ASSERT(selectors);
      
      int numFilters = selectors->size();
      ASSERT(numFilters);
      
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
          if (selectors->at(i)->hasFilter())
          {
            description += selectors->at(i)->getDescription();
            if (selectors->at(i + 1)->hasFilter())
              description += logic;
          }
        }
        
        if (selectors->at(numFilters - 1)->hasFilter())
          description += selectors->at(numFilters - 1)->getDescription();
      }
    }
    
    return description;
  }
}
