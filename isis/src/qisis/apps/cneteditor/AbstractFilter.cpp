#include "IsisDebug.h"

#include <iostream>
#include <limits>

#include "AbstractFilter.h"

#include <QAction>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFlags>
#include <QHBoxLayout>
#include <QLabel>
#include <QMargins>
#include <QMenu>
#include <QMenuBar>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <QWriteLocker>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"

#include "AbstractFilterSelector.h"


using std::cerr;



namespace Isis
{
  namespace CnetViz
  {
    AbstractFilter::AbstractFilter(FilterEffectivenessFlag effectiveness,
        int minimumForSuccess)
    {
      nullify();

      minForSuccess = minimumForSuccess;

      effectivenessFlags = new FilterEffectivenessFlag(effectiveness);

      smallFont = new QFont("SansSerif", 9);

      createWidget();
    }


    AbstractFilter::AbstractFilter(const AbstractFilter & other)
    {
      nullify();

      minForSuccess = other.minForSuccess;

      effectivenessFlags = new FilterEffectivenessFlag(*other.effectivenessFlags);

      smallFont = new QFont(*other.smallFont);

      createWidget();

      inclusiveExclusiveGroup->button(
        other.inclusiveExclusiveGroup->checkedId())->click();
    }


    AbstractFilter::~AbstractFilter()
    {
      delete effectivenessFlags;
      effectivenessFlags = NULL;

      delete inclusiveExclusiveGroup;
      inclusiveExclusiveGroup = NULL;

      delete smallFont;
      smallFont = NULL;
    }


    bool AbstractFilter::canFilterImages() const
    {
      return effectivenessFlags->testFlag(Images);
    }


    bool AbstractFilter::canFilterPoints() const
    {
      return effectivenessFlags->testFlag(Points);
    }


    bool AbstractFilter::canFilterMeasures() const
    {
      return effectivenessFlags->testFlag(Measures);
    }


    QString AbstractFilter::getImageDescription() const
    {
      return "have at least " + QString::number(getMinForSuccess()) + " ";
    }


    QString AbstractFilter::getPointDescription() const
    {
      return QString();
    }


    QString AbstractFilter::getMeasureDescription() const
    {
      return QString();
    }


    void AbstractFilter::nullify()
    {
      effectivenessGroup = NULL;
      inclusiveExclusiveGroup = NULL;
      inclusiveExclusiveLayout = NULL;
      mainLayout = NULL;
      minWidget = NULL;
      effectivenessFlags = NULL;
      smallFont = NULL;
    }


    void AbstractFilter::createWidget()
    {
      QRadioButton * inclusiveButton = new QRadioButton("Inclusive");
      inclusiveButton->setFont(*smallFont);
      QRadioButton * exclusiveButton = new QRadioButton("Exclusive");
      exclusiveButton->setFont(*smallFont);

      inclusiveExclusiveGroup = new QButtonGroup;
      connect(inclusiveExclusiveGroup, SIGNAL(buttonClicked(int)),
          this, SIGNAL(filterChanged()));
      inclusiveExclusiveGroup->addButton(inclusiveButton, 0);
      inclusiveExclusiveGroup->addButton(exclusiveButton, 1);

      inclusiveExclusiveLayout = new QHBoxLayout;
      QMargins margins = inclusiveExclusiveLayout->contentsMargins();
      margins.setTop(0);
      margins.setBottom(0);
      inclusiveExclusiveLayout->setContentsMargins(margins);
      inclusiveExclusiveLayout->addWidget(inclusiveButton);
      inclusiveExclusiveLayout->addWidget(exclusiveButton);

      QHBoxLayout * controlsLayout = new QHBoxLayout;
      margins = controlsLayout->contentsMargins();
      margins.setTop(0);
      margins.setBottom(0);
      controlsLayout->setContentsMargins(margins);

      controlsLayout->addLayout(inclusiveExclusiveLayout);

      effectivenessGroup = new QButtonGroup();
      effectivenessGroup->setExclusive(false);

      if (effectivenessFlags->testFlag(Images))
        effectivenessGroup->addButton(
            createEffectivenessCheckBox("&Images"), 0);

      if (effectivenessFlags->testFlag(Points))
        effectivenessGroup->addButton(
            createEffectivenessCheckBox("&Points"), 1);

      if (effectivenessFlags->testFlag(Measures))
        effectivenessGroup->addButton(
            createEffectivenessCheckBox("&Measures"), 2);

      QString firstGroupEntry;
      ASSERT(effectivenessGroup->buttons().size());
      if (effectivenessGroup->buttons().size())
      {
        firstGroupEntry = effectivenessGroup->buttons()[0]->text();
        firstGroupEntry.remove(0, 1);
      }

      QList<QAbstractButton *> buttons = effectivenessGroup->buttons();
      if (effectivenessGroup->buttons().size() >= 2)
      {
        QHBoxLayout * effectivenessLayout = new QHBoxLayout;
        QMargins effectivenessMargins = effectivenessLayout->contentsMargins();
        effectivenessMargins.setTop(0);
        effectivenessMargins.setBottom(0);
        effectivenessLayout->setContentsMargins(effectivenessMargins);

        for (int i = 0; i < buttons.size(); i++)
          effectivenessLayout->addWidget(buttons[i]);

        controlsLayout->addLayout(effectivenessLayout);
      }
      else
      {
        for (int i = 0; i < buttons.size(); i++)
          delete buttons[i];
        delete effectivenessGroup;
        effectivenessGroup = NULL;
      }

      if (minForSuccess != -1)
      {
        QLabel * label = new QLabel;
        label->setText(
            "<span>Min Count<br/>for " + firstGroupEntry + "</span>");
        label->setFont(QFont("SansSerif", 7));
        QSpinBox * spinBox = new QSpinBox;
        spinBox->setRange(1, std::numeric_limits< int >::max());
        spinBox->setValue(1);  // FIXME: QSettings should handle this
        connect(spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateMinForSuccess(int)));
        QHBoxLayout * minLayout = new QHBoxLayout;
        margins = minLayout->contentsMargins();
        margins.setTop(0);
        margins.setBottom(0);
        minLayout->setContentsMargins(margins);
        minLayout->addWidget(label);
        minLayout->addWidget(spinBox);
        minWidget = new QWidget;
        minWidget->setLayout(minLayout);

        controlsLayout->addWidget(minWidget);
        controlsLayout->setAlignment(minWidget, Qt::AlignTop);
        minWidget->setVisible(true); // FIXME: QSettings should handle this
      }

      controlsLayout->addStretch();

      mainLayout = new QVBoxLayout;
      margins = mainLayout->contentsMargins();
      margins.setTop(0);
      margins.setBottom(0);
      mainLayout->setContentsMargins(margins);
      mainLayout->addLayout(controlsLayout);


      setLayout(mainLayout);

      // FIXME: QSettings should handle this
      inclusiveButton->click();
    }


    QCheckBox * AbstractFilter::createEffectivenessCheckBox(QString text)
    {
      QCheckBox * checkBox = new QCheckBox(text, this);
      checkBox->setChecked(true);
      checkBox->setFont(*smallFont);
      connect(checkBox, SIGNAL(toggled(bool)),
              this, SLOT(updateEffectiveness()));
      connect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(filterChanged()));
      return checkBox;
    }


    bool AbstractFilter::inclusive() const
    {
      return inclusiveExclusiveGroup->checkedId() == 0;
    }


    AbstractFilter::FilterEffectivenessFlag *
    AbstractFilter::getEffectivenessFlags() const
    {
      return effectivenessFlags;
    }


    QBoxLayout * AbstractFilter::getMainLayout() const
    {
      ASSERT(mainLayout);

      return mainLayout;
    }


    QBoxLayout * AbstractFilter::getInclusiveExclusiveLayout() const
    {
      ASSERT(inclusiveExclusiveLayout);

      return inclusiveExclusiveLayout;
    }


    bool AbstractFilter::evaluateFromCount(QList< ControlMeasure * > measures,
        bool usePoints) const
    {
      int passedCount = 0;

      foreach(ControlMeasure * measure, measures)
      {
        ASSERT(measure);

        if (usePoints)
        {
          ControlPoint * point = measure->Parent();
          ASSERT(point);
          if (point && evaluate(point))
            passedCount++;
        }
        else
        {
          if (measure && evaluate(measure))
            passedCount++;
        }
      }

      return passedCount >= getMinForSuccess();
    }


    bool AbstractFilter::evaluateImageFromPointFilter(
      const ControlCubeGraphNode * node) const
    {
      ASSERT(node);

      bool evaluation = true;

      if (canFilterImages())
        evaluation = evaluateFromCount(node->getMeasures(), true);

      return evaluation;
    }


    bool AbstractFilter::evaluateImageFromMeasureFilter(
      const ControlCubeGraphNode * node) const
    {
      ASSERT(node);

      bool evaluation = true;

      if (canFilterImages())
        evaluation = evaluateFromCount(node->getMeasures(), false);

      return evaluation;
    }


    bool AbstractFilter::evaluatePointFromMeasureFilter(
      const ControlPoint * point) const
    {
      ASSERT(point);

      bool evaluation = true;

      if (canFilterPoints())
        evaluation = evaluateFromCount(point->getMeasures(), false);

      return evaluation;
    }


    bool AbstractFilter::evaluate(const ControlPoint * point,
        bool (ControlPoint::*meth)() const) const
    {
      ASSERT(point);

      return !((point->*meth)() ^ inclusive());
    }


    bool AbstractFilter::evaluate(const ControlMeasure * measure,
        bool (ControlMeasure::*meth)() const) const
    {
      ASSERT(measure);

      return !((measure->*meth)() ^ inclusive());
    }


    void AbstractFilter::updateEffectiveness()
    {
      ASSERT(effectivenessGroup);

      if (effectivenessGroup)
      {
        FilterEffectivenessFlag newFlags;

        QList< QAbstractButton * > buttons = effectivenessGroup->buttons();

        if (minWidget)
          minWidget->setVisible(false);

        for (int i = 0; i < buttons.size(); i++)
        {
          if (buttons[i]->isChecked())
          {
            if (buttons[i]->text() == "&Images")
              newFlags |= Images;
            else
              if (buttons[i]->text() == "&Points")
                newFlags |= Points;
              else
                if (buttons[i]->text() == "&Measures")
                  newFlags |= Measures;

            if (i == 0 && minWidget)
              minWidget->setVisible(true);
          }
        }

        *effectivenessFlags = newFlags;
      }
    }


    void AbstractFilter::updateMinForSuccess(int newMin)
    {
      minForSuccess = newMin;
      emit filterChanged();
    }
  }
}
