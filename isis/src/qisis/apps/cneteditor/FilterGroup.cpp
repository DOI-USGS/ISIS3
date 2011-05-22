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
#include "PointMeasureFilterSelector.h"
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
  
  
  bool FilterGroup::evaluate(const ControlCubeGraphNode * node) const
  {
    // if andFiltersTogether is true then we break out of the loop as soon
    // as any selectors evaluate to false.  If andFiltersTogether is false
    // then we are ORing them so we break out as soon as any selector
    // evaluates to true.  Whether we are looking for successes or failures
    // depends on whether we are ANDing or ORing the filters (selectors)
    // together!
    bool looking = true;
    for (int i = 0; looking && i < selectors->size(); i++)
      if (selectors->at(i)->hasImageFilter())
        looking = !(selectors->at(i)->evaluate(node) ^ andFiltersTogether);

    // It is good that we are still looking for failures if we were ANDing
    // filters together, but it is bad if we were ORing them since in this
    // case we were looking for success.
    return !(looking ^ andFiltersTogether) || !hasImageFilter();
  }


  bool FilterGroup::evaluate(const ControlPoint * point) const
  {
    // if andFiltersTogether is true then we break out of the loop as soon
    // as any selectors evaluate to false.  If andFiltersTogether is false
    // then we are ORing them so we break out as soon as any selector
    // evaluates to true.  Whether we are looking for successes or failures
    // depends on whether we are ANDing or ORing the filters (selectors)
    // together!
    bool looking = true;
    for (int i = 0; looking && i < selectors->size(); i++)
      if (selectors->at(i)->hasPointFilter())
        looking = !(selectors->at(i)->evaluate(point) ^ andFiltersTogether);

    // It is good that we are still looking for failures if we were ANDing
    // filters together, but it is bad if we were ORing them since in this
    // case we were looking for success.
    return !(looking ^ andFiltersTogether) || !hasPointFilter();
  }

  bool FilterGroup::evaluate(const ControlMeasure * measure) const
  {
    // if andFiltersTogether is true then we break out of the loop as soon
    // as any selectors evaluate to false.  If andFiltersTogether is false
    // then we are ORing them so we break out as soon as any selector
    // evaluates to true.  Whether we are looking for successes or failures
    // depends on whether we are ANDing or ORing the filters (selectors)
    // together!
    bool looking = true;
    for (int i = 0; looking && i < selectors->size(); i++)
      if (selectors->at(i)->hasMeasureFilter())
        looking = !(selectors->at(i)->evaluate(measure) ^ andFiltersTogether);

    // It is good that we are still looking for failures if we were ANDing
    // filters together, but it is bad if we were ORing them since in this
    // case we were looking for success.
    return !(looking ^ andFiltersTogether) || !hasMeasureFilter();
  }

  
  
  bool FilterGroup::hasFilter() const
  {
    return hasSelectorWithCondition(&AbstractFilterSelector::hasFilter);
  }
  
  
  bool FilterGroup::hasImageFilter() const
  {
    return hasSelectorWithCondition(&AbstractFilterSelector::hasImageFilter);
  }
  
  
  bool FilterGroup::hasPointFilter() const
  {
    return hasSelectorWithCondition(&AbstractFilterSelector::hasPointFilter);
  }
  
  
  bool FilterGroup::hasMeasureFilter() const
  {
    return hasSelectorWithCondition(&AbstractFilterSelector::hasMeasureFilter);
  }
  

  bool FilterGroup::hasSelectorWithCondition(
      bool (AbstractFilterSelector::*meth)() const) const
  {
    bool found = false;
    
    for (int i = 0; !found && i < selectors->size(); i++)
      found = (selectors->at(i)->*meth)();
    
    return found;
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
    if (filterType == "Points and Measures")
      filterSelector = new PointMeasureFilterSelector;
    else
      if (filterType == "Images and Points")
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
      connect(filterSelector, SIGNAL(sizeChanged()),
          this, SLOT(sendSizeChanged()));
      groupBoxLayout->insertWidget(groupBoxLayout->count() - 1, filterSelector);
      selectors->append(filterSelector);
      selectors->size() > 1 ? logicWidget->show() : logicWidget->hide();
    }
    
    sendSizeChanged();
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
  
  
  QString FilterGroup::getDescription(
      bool (AbstractFilterSelector::*meth)() const) const
  {
    QString description;
    
    ASSERT(selectors);
      
    QList< AbstractFilterSelector * > selectorsWithFilters;
    for (int i = 0; i < selectors->size(); i++)
      if ((selectors->at(i)->*meth)())
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
        description += selectorsWithFilters[i]->getDescription() + logic;
      
      description += selectorsWithFilters[numFilters - 1]->getDescription();
    }
    
    return description;
  }
  
  
  QString FilterGroup::getImageDescription() const
  {
    return getDescription(&AbstractFilterSelector::hasImageFilter);
  }
  
  
  QString FilterGroup::getPointDescription() const
  {
    return getDescription(&AbstractFilterSelector::hasPointFilter);
  }
  
  
  QString FilterGroup::getMeasureDescription() const
  {
    return getDescription(&AbstractFilterSelector::hasMeasureFilter);
  }
}
