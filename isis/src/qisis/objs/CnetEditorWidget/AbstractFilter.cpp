/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
#include <QPair>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <QWriteLocker>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

#include "AbstractFilterSelector.h"


namespace Isis {
  AbstractFilter::AbstractFilter(FilterEffectivenessFlag effectiveness,
                                 int minimumForSuccess) {
    nullify();

    m_minForSuccess = minimumForSuccess;

    m_effectivenessFlags = new FilterEffectivenessFlag(effectiveness);

    m_smallFont = new QFont("SansSerif", 9);

    createWidget();
  }


  AbstractFilter::AbstractFilter(const AbstractFilter &other) {
    nullify();

    m_minForSuccess = other.m_minForSuccess;

    m_effectivenessFlags = new FilterEffectivenessFlag(*other.m_effectivenessFlags);

    m_smallFont = new QFont(*other.m_smallFont);

    createWidget();

    m_inclusiveExclusiveGroup->button(
      other.m_inclusiveExclusiveGroup->checkedId())->click();
  }


  AbstractFilter::~AbstractFilter() {
    delete m_effectivenessFlags;
    m_effectivenessFlags = NULL;

    delete m_inclusiveExclusiveGroup;
    m_inclusiveExclusiveGroup = NULL;

    delete m_smallFont;
    m_smallFont = NULL;
  }


  bool AbstractFilter::canFilterImages() const {
    return m_effectivenessFlags->testFlag(Images);
  }


  bool AbstractFilter::canFilterPoints() const {
    return m_effectivenessFlags->testFlag(Points);
  }


  bool AbstractFilter::canFilterMeasures() const {
    return m_effectivenessFlags->testFlag(Measures);
  }


  QString AbstractFilter::getImageDescription() const {
    return "have at least " + QString::number(getMinForSuccess()) + " ";
  }


  QString AbstractFilter::getPointDescription() const {
    return QString();
  }


  QString AbstractFilter::getMeasureDescription() const {
    return QString();
  }


  void AbstractFilter::nullify() {
    m_effectivenessGroup = NULL;
    m_inclusiveExclusiveGroup = NULL;
    m_inclusiveExclusiveLayout = NULL;
    m_mainLayout = NULL;
    m_minWidget = NULL;
    m_effectivenessFlags = NULL;
    m_smallFont = NULL;
  }


  void AbstractFilter::createWidget() {
    QRadioButton *inclusiveButton = new QRadioButton("Inclusive");
    inclusiveButton->setFont(*m_smallFont);
    QRadioButton *exclusiveButton = new QRadioButton("Exclusive");
    exclusiveButton->setFont(*m_smallFont);

    m_inclusiveExclusiveGroup = new QButtonGroup;
    connect(m_inclusiveExclusiveGroup, SIGNAL(buttonClicked(int)),
        this, SIGNAL(filterChanged()));
    m_inclusiveExclusiveGroup->addButton(inclusiveButton, 0);
    m_inclusiveExclusiveGroup->addButton(exclusiveButton, 1);

    m_inclusiveExclusiveLayout = new QHBoxLayout;
    QMargins margins = m_inclusiveExclusiveLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    m_inclusiveExclusiveLayout->setContentsMargins(margins);
    m_inclusiveExclusiveLayout->addWidget(inclusiveButton);
    m_inclusiveExclusiveLayout->addWidget(exclusiveButton);

    QHBoxLayout *controlsLayout = new QHBoxLayout;
    margins = controlsLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    controlsLayout->setContentsMargins(margins);

    controlsLayout->addLayout(m_inclusiveExclusiveLayout);

    m_effectivenessGroup = new QButtonGroup();
    m_effectivenessGroup->setExclusive(false);

    if (m_effectivenessFlags->testFlag(Images))
      m_effectivenessGroup->addButton(
        createEffectivenessCheckBox("&Images"), 0);

    if (m_effectivenessFlags->testFlag(Points))
      m_effectivenessGroup->addButton(
        createEffectivenessCheckBox("&Points"), 1);

    if (m_effectivenessFlags->testFlag(Measures))
      m_effectivenessGroup->addButton(
        createEffectivenessCheckBox("&Measures"), 2);

    QString firstGroupEntry;

    if (m_effectivenessGroup->buttons().size()) {
      firstGroupEntry = m_effectivenessGroup->buttons()[0]->text();
      firstGroupEntry.remove(0, 1);
    }

    QList<QAbstractButton *> buttons = m_effectivenessGroup->buttons();
    if (m_effectivenessGroup->buttons().size() >= 2) {
      QHBoxLayout *effectivenessLayout = new QHBoxLayout;
      QMargins effectivenessMargins = effectivenessLayout->contentsMargins();
      effectivenessMargins.setTop(0);
      effectivenessMargins.setBottom(0);
      effectivenessLayout->setContentsMargins(effectivenessMargins);

      for (int i = 0; i < buttons.size(); i++)
        effectivenessLayout->addWidget(buttons[i]);

      controlsLayout->addLayout(effectivenessLayout);
    }
    else {
      for (int i = 0; i < buttons.size(); i++)
        delete buttons[i];
      delete m_effectivenessGroup;
      m_effectivenessGroup = NULL;
    }

    if (m_minForSuccess != -1) {
      QLabel *label = new QLabel;
      label->setText(
        "<span>Min Count<br/>for " + firstGroupEntry + "</span>");
      label->setFont(QFont("SansSerif", 7));
      QSpinBox *spinBox = new QSpinBox;
      spinBox->setRange(1, std::numeric_limits< int >::max());
      spinBox->setValue(1);  // FIXME: QSettings should handle this
      connect(spinBox, SIGNAL(valueChanged(int)),
          this, SLOT(updateMinForSuccess(int)));
      QHBoxLayout *minLayout = new QHBoxLayout;
      margins = minLayout->contentsMargins();
      margins.setTop(0);
      margins.setBottom(0);
      minLayout->setContentsMargins(margins);
      minLayout->addWidget(label);
      minLayout->addWidget(spinBox);
      m_minWidget = new QWidget;
      m_minWidget->setLayout(minLayout);

      controlsLayout->addWidget(m_minWidget);
      controlsLayout->setAlignment(m_minWidget, Qt::AlignTop);
      m_minWidget->setVisible(true); // FIXME: QSettings should handle this
    }

    controlsLayout->addStretch();

    m_mainLayout = new QVBoxLayout;
    margins = m_mainLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    m_mainLayout->setContentsMargins(margins);
    m_mainLayout->addLayout(controlsLayout);


    setLayout(m_mainLayout);

    // FIXME: QSettings should handle this
    inclusiveButton->click();
  }


  QCheckBox *AbstractFilter::createEffectivenessCheckBox(QString text) {
    QCheckBox *checkBox = new QCheckBox(text, this);
    checkBox->setChecked(true);
    checkBox->setFont(*m_smallFont);
    connect(checkBox, SIGNAL(toggled(bool)),
        this, SLOT(updateEffectiveness()));
    connect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(filterChanged()));
    return checkBox;
  }


  bool AbstractFilter::inclusive() const {
    return m_inclusiveExclusiveGroup->checkedId() == 0;
  }


  AbstractFilter::FilterEffectivenessFlag *
  AbstractFilter::getEffectivenessFlags() const {
    return m_effectivenessFlags;
  }


  QBoxLayout *AbstractFilter::getMainLayout() const {

    return m_mainLayout;
  }


  QBoxLayout *AbstractFilter::getInclusiveExclusiveLayout() const {

    return m_inclusiveExclusiveLayout;
  }


  bool AbstractFilter::evaluateFromCount(QList< ControlMeasure * > measures,
      bool usePoints) const {
    int passedCount = 0;

    foreach (ControlMeasure * measure, measures) {

      if (usePoints) {
        ControlPoint *point = measure->Parent();

        if (point && evaluate(point))
          passedCount++;
      }
      else {
        if (measure && evaluate(measure))
          passedCount++;
      }
    }

    return passedCount >= getMinForSuccess();
  }


  bool AbstractFilter::evaluateImageFromPointFilter(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    bool evaluation = true;

    if (canFilterImages()) {
      evaluation = evaluateFromCount(imageAndNet->second->GetMeasuresInCube(imageAndNet->first),
                                     true);
    }

    return evaluation;
  }


  bool AbstractFilter::evaluateImageFromMeasureFilter(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    bool evaluation = true;

    if (canFilterImages()) {
      evaluation = evaluateFromCount(imageAndNet->second->GetMeasuresInCube(imageAndNet->first),
                                     false);
    }

    return evaluation;
  }


  bool AbstractFilter::evaluatePointFromMeasureFilter(
    const ControlPoint *point) const {

    bool evaluation = true;

    if (canFilterPoints())
      evaluation = evaluateFromCount(point->getMeasures(), false);

    return evaluation;
  }


  bool AbstractFilter::evaluate(const ControlPoint *point,
      bool (ControlPoint::*meth)() const) const {

    return !((point->*meth)() ^ inclusive());
  }


  bool AbstractFilter::evaluate(const ControlMeasure *measure,
      bool (ControlMeasure::*meth)() const) const {

    return !((measure->*meth)() ^ inclusive());
  }


  void AbstractFilter::updateEffectiveness() {

    if (m_effectivenessGroup) {
      FilterEffectivenessFlag newFlags;

      QList< QAbstractButton * > buttons = m_effectivenessGroup->buttons();

      if (m_minWidget)
        m_minWidget->setVisible(false);

      for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->isChecked()) {
          if (buttons[i]->text() == "&Images")
            newFlags |= Images;
          else if (buttons[i]->text() == "&Points")
            newFlags |= Points;
          else if (buttons[i]->text() == "&Measures")
            newFlags |= Measures;

          if (i == 0 && m_minWidget)
            m_minWidget->setVisible(true);
        }
      }

      *m_effectivenessFlags = newFlags;
    }
  }


  void AbstractFilter::updateMinForSuccess(int newMin) {
    m_minForSuccess = newMin;
    emit filterChanged();
  }
}
