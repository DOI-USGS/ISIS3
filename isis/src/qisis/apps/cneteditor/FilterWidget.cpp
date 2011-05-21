#include "IsisDebug.h"

#include "FilterWidget.h"

#include <iostream>

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>

#include "FilterGroup.h"

using std::cerr;

namespace Isis
{
  FilterWidget::FilterWidget(QString type)
  {
    nullify();
    
    filterGroups = new QList< FilterGroup * >;

    QString whatsThis = "Filters are organized into groups.  Filters within a "
        "group will be combined using either AND or OR logic.  Furthermore, "
        "multiple groups are supported, and the logic used to combine the "
        "various groups is also configurable.  To illustrate what this allows "
        "consider an example.  Let A, B, and C be (filters).  By creating two "
        "groups, one with A and B in it and the other with C in it, it is "
        "possible to build the expression \"(A and B) or C\".";
    
    filterType = type;
    QString title = "Filter " + type;
    QLabel * titleLabel = new QLabel(title);
    titleLabel->setFont(QFont("SansSerif", 15, QFont::DemiBold));
    
    QHBoxLayout * titleLayout = new QHBoxLayout;
    titleLayout->addStretch();
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    
    QLabel * logicTypeLabel = new QLabel(
        "Combine groups using logic type: ");
    QFont logicTypeLabelFont("SansSerif", 12);
    logicTypeLabel->setFont(logicTypeLabelFont);

    QFont logicTypeFont("SansSerif", 12, QFont::Bold);
    QRadioButton * andButton = new QRadioButton("AND");
    andButton->setFont(logicTypeFont);
    QRadioButton * orButton = new QRadioButton("OR");
    orButton->setFont(logicTypeFont);
    QButtonGroup * buttonGroup = new QButtonGroup;
    buttonGroup->addButton(andButton, 0);
    buttonGroup->addButton(orButton, 1);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this,
        SLOT(changeGroupCombinationLogic(int)));
    
    // FIXME: this should be controlled by QSettings
    orButton->click();

    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(logicTypeLabel);
    buttonLayout->addWidget(andButton);
    buttonLayout->addWidget(orButton);
    buttonLayout->addStretch();
    logicWidget = new QWidget;
    logicWidget->setLayout(buttonLayout);
    
    addGroupButton = new QPushButton;
    addGroupButton->setIcon(QIcon(":add"));
    addGroupButton->setToolTip("Add Filter Group");
    addGroupButton->setWhatsThis(whatsThis);
    connect(addGroupButton, SIGNAL(clicked()), this, SLOT(addGroup()));
    QHBoxLayout * addGroupLayout = new QHBoxLayout;
    addGroupLayout->addWidget(addGroupButton);
    addGroupLayout->addStretch();
    
    description = new QLabel;
    description->setWordWrap(true);
    description->setFont(QFont("SansSerif", 13));
    connect(this, SIGNAL(filterChanged()), this, SLOT(updateDescription()));

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(titleLayout);
    mainLayout->addWidget(description);
    mainLayout->addWidget(logicWidget);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(addGroupLayout);
    mainLayout->addStretch();
    
    setLayout(mainLayout);
    setWhatsThis(whatsThis);
    
    addGroup();
  }


  FilterWidget::~FilterWidget()
  {
    if (filterGroups)
    {
      delete filterGroups;
      filterGroups = NULL;
    }
  }
  
  
  bool FilterWidget::hasFilter() const
  {
    bool foundFilter = false;
    if (filterGroups)
      for (int i = 0; !foundFilter && i < filterGroups->size(); i++)
        foundFilter = filterGroups->at(i)->hasFilter();
    
    return foundFilter;
  }


  void FilterWidget::nullify()
  {
    addGroupButton = NULL;
    mainLayout = NULL;
    filterGroups = NULL;
  }
  
  
  void FilterWidget::updateDescription()
  {
    ASSERT(description);
    if (description)
    {
      QString newText = "<font color=black>Showing ";
      description->clear();
      
      if (hasFilter())
      {
        ASSERT(filterGroups);
        if (filterGroups->size())
        {
          newText += filterType + " which </font>";
          
          QString groupLogic;
          if (andGroupsTogether)
            groupLogic += " AND ";
          else
            groupLogic += " OR ";
          
          int numGroups = filterGroups->size();
          ASSERT(numGroups);
          for (int i = 0; i < numGroups - 1; i++)
          {
            if (numGroups > 1)
              newText += "<font color=black><b>(</b></font>";
            newText += "<font color=darkBlue>";
            newText += filterGroups->at(i)->getDescription() + "</font>";
            if (numGroups > 1)
              newText += "<font color=black><b>)</b></font>";
            newText += "<font color=black><b>" + groupLogic + "</b></font>";
          }
          
          if (numGroups > 1)
            newText += "<font color=black><b>(</b></font>";
          newText += "<font color=darkBlue>";
          newText += filterGroups->at(numGroups - 1)->getDescription();
          newText += "</font>";
          if (numGroups > 1)
            newText += "<font color=black><b>)</b></font>";
          newText += "<font color=black>.</font>";
        }
      }
      else
      {
        newText += "all " + filterType + ".</font>";
      }
      
      description->setText(newText);
    }
  }


  void FilterWidget::addGroup()
  {
    FilterGroup * newGroup = new FilterGroup(filterType);
    connect(newGroup, SIGNAL(close(FilterGroup *)),
        this, SLOT(deleteGroup(FilterGroup *)));
    connect(newGroup, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
    mainLayout->insertWidget(mainLayout->count() - 2, newGroup);
    filterGroups->append(newGroup);
    filterGroups->size() > 1 ? logicWidget->show() : logicWidget->hide();
  }


  void FilterWidget::deleteGroup(FilterGroup * filterGroup)
  {
    mainLayout->removeWidget(filterGroup);
    delete filterGroup;
    filterGroups->removeOne(filterGroup);
    filterGroups->size() > 1 ? logicWidget->show() : logicWidget->hide();
    emit filterChanged();
  }
  
  
  void FilterWidget::changeGroupCombinationLogic(int button)
  {
    andGroupsTogether = button == 0;
    emit filterChanged();
  }
}
