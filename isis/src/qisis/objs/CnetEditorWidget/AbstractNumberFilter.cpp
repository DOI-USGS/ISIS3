/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractNumberFilter.h"

#include <iostream>

#include <QButtonGroup>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMargins>
#include <QRadioButton>

#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis {
  AbstractNumberFilter::AbstractNumberFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
    nullify();
    createWidget();
  }


  AbstractNumberFilter::AbstractNumberFilter(const AbstractNumberFilter &other)
        : AbstractFilter(other) {
    nullify();
    createWidget();

    m_lineEdit->setText(other.m_lineEdit->text());
    m_greaterThanLessThan->button(
      other.m_greaterThanLessThan->checkedId())->click();
  }



  AbstractNumberFilter::~AbstractNumberFilter() {
    delete m_lineEditText;
    m_lineEditText = NULL;
  }


  void AbstractNumberFilter::nullify() {
    m_greaterThanLessThan = NULL;
    m_lineEdit = NULL;
    m_lineEditText = NULL;
  }


  void AbstractNumberFilter::createWidget() {
    QFont greaterThanLessThanFont("SansSerif", 9);

    QRadioButton *lessThanButton = new QRadioButton("<=");
    lessThanButton->setFont(greaterThanLessThanFont);
    QRadioButton *greaterThanButton = new QRadioButton(">=");
    greaterThanButton->setFont(greaterThanLessThanFont);

    m_greaterThanLessThan = new QButtonGroup;
    connect(m_greaterThanLessThan, SIGNAL(buttonClicked(int)),
        this, SIGNAL(filterChanged()));
    m_greaterThanLessThan->addButton(lessThanButton, 0);
    m_greaterThanLessThan->addButton(greaterThanButton, 1);

    // hide inclusive and exclusive buttons, and add greater than and less than
    // in the inclusiveExclusiveLayout.
    getInclusiveExclusiveLayout()->itemAt(0)->widget()->setVisible(false);
    getInclusiveExclusiveLayout()->itemAt(1)->widget()->setVisible(false);
    getInclusiveExclusiveLayout()->addWidget(lessThanButton);
    getInclusiveExclusiveLayout()->addWidget(greaterThanButton);

    m_lineEditText = new QString;

    m_lineEdit = new QLineEdit;
    m_lineEdit->setMinimumWidth(75);
    connect(m_lineEdit, SIGNAL(textChanged(QString)),
        this, SLOT(updateLineEditText(QString)));
    connect(m_lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));


    QHBoxLayout *layout = new QHBoxLayout;
    QMargins margins = layout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    layout->setContentsMargins(margins);
    layout->addWidget(m_lineEdit);
    layout->addStretch();

    getMainLayout()->addLayout(layout);

    // FIXME: QSettings should handle this
    lessThanButton->click();
  }


  bool AbstractNumberFilter::evaluate(double number) const {
    bool evaluation = true;

    // multiple threads reading the m_lineEditText so lock it
    QString text = *m_lineEditText;

    bool ok = false;
    double d = text.toDouble(&ok);

    if (ok)
      evaluation = !(inclusive() ^ lessThan() ^(d <= number));

    return evaluation;
  }


  QString AbstractNumberFilter::descriptionSuffix() const {
    QString suffix;

    if (!inclusive())
      suffix += "not ";

    if (lessThan())
      suffix += "less than or equal to \"";
    else
      suffix += "greater than or equal to \"";

    suffix += *m_lineEditText;

    suffix += "\"";

    return suffix;
  }


  bool AbstractNumberFilter::lessThan() const {
    return m_greaterThanLessThan->checkedId() == 0;
  }


  void AbstractNumberFilter::updateLineEditText(QString newText) {
    *m_lineEditText = newText;
  }
}
