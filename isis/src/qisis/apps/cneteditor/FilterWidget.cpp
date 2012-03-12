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

#include "IException.h"
#include "iString.h"

#include "FilterGroup.h"


using std::cerr;
using std::swap;
using std::nothrow;


namespace Isis
{
  namespace CnetViz
  {
    FilterWidget::FilterWidget(QString type)
    {
      nullify();

      filterType = new QString(type);

      init();
      addGroup();
      updateDescription();
    }


    FilterWidget::FilterWidget(const FilterWidget & other)
    {
      nullify();

      filterType = new QString(*other.filterType);

      init();

      foreach(FilterGroup * group, *other.filterGroups)
      addGroup(new FilterGroup(*group));

      buttonGroup->button(other.buttonGroup->checkedId())->click();

      updateDescription();
    }


    FilterWidget::~FilterWidget()
    {
      if (buttonGroup)
      {
        delete buttonGroup;
        buttonGroup = NULL;
      }

      if (filterGroups)
      {
        delete filterGroups;
        filterGroups = NULL;
      }

      if (filterType)
      {
        delete filterType;
        filterType = NULL;
      }
    }


    bool FilterWidget::evaluate(const ControlCubeGraphNode * node) const
    {
      return evaluate(node, &AbstractFilter::canFilterImages);
    }


    bool FilterWidget::evaluate(const ControlPoint * point) const
    {
      return evaluate(point, &AbstractFilter::canFilterPoints);
    }


    bool FilterWidget::evaluate(const ControlMeasure * measure) const
    {
      return evaluate(measure, &AbstractFilter::canFilterMeasures);
    }


    bool FilterWidget::hasFilter(bool (AbstractFilter::*meth)() const) const
    {
      bool found = false;

      for (int i = 0; !found && i < filterGroups->size(); i++)
        found = filterGroups->at(i)->hasFilter(meth);

      return found;
    }


    FilterWidget & FilterWidget::operator=(FilterWidget other)
    {
      ASSERT(filterType);
      ASSERT(filterGroups);
      ASSERT(buttonGroup);

      // create temporary list of new groups
      QList< FilterGroup * > newGroups;
      foreach(FilterGroup * group, *other.filterGroups)
      {
        FilterGroup * newGroup = new(nothrow) FilterGroup(*other.filterType);
        if (newGroup)
        {
          *newGroup = *group;
          newGroups.append(newGroup);
        }
      }

      // if all is ok, and it is safe to assign
      if (newGroups.size() == other.filterGroups->size())
      {
        foreach(FilterGroup * group, *filterGroups)
        {
          deleteGroup(group);
        }

        foreach(FilterGroup * newGroup, newGroups)
        {
          addGroup(newGroup);
        }

        swap(*filterType, *other.filterType);
        buttonGroup->button(other.buttonGroup->checkedId())->click();
      }
      else
      {
        // clean up any temp groups
        foreach(FilterGroup * newGroup, newGroups)
        {
          if (newGroup)
          {
            delete newGroup;
            newGroup = NULL;
          }
        }

        iString msg = "Assignment of FilterWidget failed";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      return *this;
    }


    void FilterWidget::nullify()
    {
      addGroupButton = NULL;
      buttonGroup = NULL;
      imageDescription = NULL;
      pointDescription = NULL;
      measureDescription = NULL;
      mainLayout = NULL;
      filterGroups = NULL;
      filterType = NULL;
    }


    void FilterWidget::init()
    {
      filterGroups = new QList< FilterGroup * >;

      QString whatsThis = "<html>Filters are organized into groups (bounded by a box)."
          "  Filters within a group will be combined using either AND or OR "
          "logic.  Furthermore, multiple groups are supported, and the logic "
          "used to combine the various groups is also configurable.<br/><br/>"
          "For example, let A, B, and C be filters.  By creating two "
          "groups, one with A and B and the other with C, it is possible to "
          "build the expression \"(A and B) or C\".<br/><br/>"
          "Each group has a green plus (+) button, which adds a new filter to "
          "the group.  There is also a green plus (+) button outside any group "
          "for adding a new group.</html>";

      ASSERT(filterType);

      QString title = "Filter " + *filterType;
      QLabel * titleLabel = new QLabel(title);
      titleLabel->setFont(QFont("SansSerif", 15, QFont::DemiBold));

      QHBoxLayout * titleLayout = new QHBoxLayout;
      titleLayout->addStretch();
      titleLayout->addWidget(titleLabel);
      titleLayout->addStretch();

      QLabel * logicTypeLabel = new QLabel("Combine groups using logic type: ");
      QFont logicTypeLabelFont("SansSerif", 12);
      logicTypeLabel->setFont(logicTypeLabelFont);

      QFont logicTypeFont("SansSerif", 12, QFont::Bold);
      QRadioButton * andButton = new QRadioButton("AND");
      andButton->setFont(logicTypeFont);
      QRadioButton * orButton = new QRadioButton("OR");
      orButton->setFont(logicTypeFont);
      buttonGroup = new QButtonGroup;
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
      QString addGroupTooltip = "Add new filter group";
      addGroupButton->setToolTip(addGroupTooltip);
      addGroupButton->setStatusTip(addGroupTooltip);
      addGroupButton->setWhatsThis(whatsThis);
      connect(addGroupButton, SIGNAL(clicked()), this, SLOT(addGroup()));
      QHBoxLayout * addGroupLayout = new QHBoxLayout;
      addGroupLayout->addWidget(addGroupButton);
      addGroupLayout->addStretch();

      QLabel * titleDummy = new QLabel;
      titleDummy->setFont(QFont("SansSerif", 6)); // FIXME

      imageDescription = new QLabel;
      imageDescription->setWordWrap(true);
      imageDescription->setFont(QFont("SansSerif", 10)); // FIXME

      pointDescription = new QLabel;
      pointDescription->setWordWrap(true);
      pointDescription->setFont(QFont("SansSerif", 10)); // FIXME

      measureDescription = new QLabel;
      measureDescription->setWordWrap(true);
      measureDescription->setFont(QFont("SansSerif", 10)); // FIXME

      QVBoxLayout * descriptionLayout = new QVBoxLayout;
      descriptionLayout->addWidget(titleDummy);
      descriptionLayout->addWidget(imageDescription);
      descriptionLayout->addWidget(pointDescription);
      descriptionLayout->addWidget(measureDescription);

      connect(this, SIGNAL(filterChanged()),
          this, SLOT(updateDescription()));

      mainLayout = new QVBoxLayout;
      mainLayout->addLayout(titleLayout);
      mainLayout->addLayout(descriptionLayout);
      mainLayout->addWidget(logicWidget);
      mainLayout->addLayout(addGroupLayout);
      mainLayout->addStretch();

      setLayout(mainLayout);
      setWhatsThis(whatsThis);
    }


    void FilterWidget::updateDescription()
    {
      updateDescription(imageDescription, &AbstractFilter::canFilterImages,
          &AbstractFilter::getImageDescription, "images");
      updateDescription(pointDescription, &AbstractFilter::canFilterPoints,
          &AbstractFilter::getPointDescription, "points");
      updateDescription(measureDescription, &AbstractFilter::canFilterMeasures,
          &AbstractFilter::getMeasureDescription, "measures");
    }


    void FilterWidget::updateDescription(QLabel * label,
        bool (AbstractFilter::*hasFilterMeth)() const,
        QString(AbstractFilter::*descriptionMeth)() const,
        QString title)
    {
      ASSERT(label);

      if (label)
      {
        label->clear();

        QList< FilterGroup * > groups;
        for (int i = 0; i < filterGroups->size(); i++)
          if (filterGroups->at(i)->hasFilter(hasFilterMeth))
            groups.append(filterGroups->at(i));

        const int GROUP_SIZE = groups.size();

        if (GROUP_SIZE)
        {
          QString black = "<font color=black>";
          QString blue = "<font color=darkBlue>";
          QString red = "<font color=darkRed>";
          QString end = "</font>";

          QString text = "Showing " + red + title + end + black + " which " + end;

          QString groupLogic;
          if (andGroupsTogether)
            groupLogic += " AND ";
          else
            groupLogic += " OR ";

          QString leftParen = black + "<b>(</b>" + end;
          QString rightParen = black + "<b>)</b>" + end;

          for (int i = 0; i < GROUP_SIZE - 1; i++)
          {
            if (GROUP_SIZE > 1)
              text += leftParen;
            text += blue + groups[i]->getDescription(
                hasFilterMeth, descriptionMeth) + end;
            if (GROUP_SIZE > 1)
              text += rightParen + black + "<b>" + groupLogic + "</b>" + end;
          }

          if (GROUP_SIZE > 1)
            text += leftParen;
          text += blue + groups[GROUP_SIZE - 1]->getDescription(
              hasFilterMeth, descriptionMeth) + end;
          if (GROUP_SIZE > 1)
            text += rightParen;

          text += black + "." + end;

          label->setText(text);
        }
      }
    }


    void FilterWidget::maybeScroll(FilterGroup * group)
    {
      ASSERT(filterGroups);
      ASSERT(filterGroups->size());

      if (filterGroups && filterGroups->size() &&
          filterGroups->at(filterGroups->size() - 1) == group)
        emit scrollToBottom();
    }


    void FilterWidget::addGroup()
    {
      FilterGroup * newGroup = new FilterGroup(*filterType);
      addGroup(newGroup);
    }


    void FilterWidget::addGroup(FilterGroup * newGroup)
    {
      connect(newGroup, SIGNAL(close(FilterGroup *)),
          this, SLOT(deleteGroup(FilterGroup *)));
      connect(newGroup, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
      connect(newGroup, SIGNAL(sizeChanged(FilterGroup *)),
          this, SLOT(maybeScroll(FilterGroup *)));
      mainLayout->insertWidget(mainLayout->count() - 2, newGroup);
      filterGroups->append(newGroup);
      filterGroups->size() > 1 ? logicWidget->show() : logicWidget->hide();

      emit scrollToBottom();
      emit filterChanged();
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
}
