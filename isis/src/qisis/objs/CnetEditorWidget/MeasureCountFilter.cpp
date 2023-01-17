/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "MeasureCountFilter.h"

#include <iostream>

#include <QButtonGroup>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMargins>
#include <QPair>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  MeasureCountFilter::MeasureCountFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
    init();
    createWidget();
  }


  MeasureCountFilter::MeasureCountFilter(const MeasureCountFilter &other) : AbstractFilter(other) {
    init();
    createWidget();

    m_count = other.m_count;
    m_minimum = other.m_minimum;
    m_countSpinBox->setValue(other.m_countSpinBox->value());
    m_minMaxGroup->button(other.m_minMaxGroup->checkedId())->click();
  }



  MeasureCountFilter::~MeasureCountFilter() {
  }


  void MeasureCountFilter::init() {
    m_minMaxGroup = NULL;
    m_countSpinBox = NULL;
    m_count = 0;
    m_minimum = true;
  }


  void MeasureCountFilter::createWidget() {
    QFont minMaxFont("SansSerif", 9);
    QRadioButton *minButton = new QRadioButton("Minimum");
    minButton->setFont(minMaxFont);
    QRadioButton *maxButton = new QRadioButton("Maximum");
    maxButton->setFont(minMaxFont);

    m_minMaxGroup = new QButtonGroup;
    connect(m_minMaxGroup, SIGNAL(buttonClicked(int)),
        this, SLOT(updateMinMax(int)));
    m_minMaxGroup->addButton(minButton, 0);
    m_minMaxGroup->addButton(maxButton, 1);

    minButton->click();

    m_countSpinBox = new QSpinBox;
    m_countSpinBox->setRange(0, std::numeric_limits< int >::max());
    m_countSpinBox->setValue(m_count);
    connect(m_countSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(updateMeasureCount(int)));

    // hide inclusive and exclusive buttons,
    // and add spinbox for min measure m_count
    getInclusiveExclusiveLayout()->itemAt(0)->widget()->setVisible(false);
    getInclusiveExclusiveLayout()->itemAt(1)->widget()->setVisible(false);
    getInclusiveExclusiveLayout()->addWidget(minButton);
    getInclusiveExclusiveLayout()->addWidget(maxButton);
    getInclusiveExclusiveLayout()->addSpacing(8);
    getInclusiveExclusiveLayout()->addWidget(m_countSpinBox);
  }


  bool MeasureCountFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromPointFilter(imageAndNet);
  }


  bool MeasureCountFilter::evaluate(const ControlPoint *point) const {
    return (point->getMeasures().size() >= m_count && m_minimum) ||
           (point->getMeasures().size() <= m_count && !m_minimum);
  }


  bool MeasureCountFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *MeasureCountFilter::clone() const {
    return new MeasureCountFilter(*this);
  }


  QString MeasureCountFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point that ";

      if (!inclusive())
        description += "doesn't have";

      description += "has ";
    }
    else {
      description += "points that ";

      if (!inclusive())
        description += "don't ";

      description += "have ";
    }

    description += "at ";

    if (m_minimum)
      description += "least ";
    else
      description += "most ";

    description += QString::number(m_count) + " measures";

    return description;
  }


  QString MeasureCountFilter::getPointDescription() const {
    QString description;

    if (!inclusive())
      description = "don't ";

    description = "have at ";

    if (m_minimum)
      description += "least ";
    else
      description += "most ";

    description += QString::number(m_count) + " measures";

    return description;
  }


  void MeasureCountFilter::updateMinMax(int buttonId) {
    m_minimum = (buttonId == 0);
    emit filterChanged();
  }


  void MeasureCountFilter::updateMeasureCount(int newCount) {
    m_count = newCount;
    emit filterChanged();
  }
}
