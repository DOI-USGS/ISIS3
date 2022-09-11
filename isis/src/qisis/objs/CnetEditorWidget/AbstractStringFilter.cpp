/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractStringFilter.h"

#include <iostream>

#include <QHBoxLayout>
#include <QLineEdit>

#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis {
  AbstractStringFilter::AbstractStringFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
    nullify();
    createWidget();
  }

  AbstractStringFilter::AbstractStringFilter(
        const AbstractStringFilter &other) : AbstractFilter(other) {
    nullify();
    createWidget();

    m_lineEdit->setText(other.m_lineEdit->text());
  }



  AbstractStringFilter::~AbstractStringFilter() {
    if (m_lineEditText) {
      delete m_lineEditText;
      m_lineEditText = NULL;
    }
  }


  void AbstractStringFilter::nullify() {
    m_lineEdit = NULL;
    m_lineEditText = NULL;
  }


  void AbstractStringFilter::createWidget() {
    m_lineEditText = new QString;

    m_lineEdit = new QLineEdit;
    m_lineEdit->setMinimumWidth(250);
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
  }


  bool AbstractStringFilter::evaluate(QString str) const {
    bool evaluation = true;

    // multiple threads reading the lineEditText so lock it
    QString text = *m_lineEditText;

    if (text.size() >= 1) {
      bool match = str.contains(text, Qt::CaseInsensitive);

      // inclusive() and match must either be both true or both false
      evaluation = !(inclusive() ^ match);
    }

    return evaluation;
  }


  QString AbstractStringFilter::descriptionSuffix() const {
    QString suffix;

    if (inclusive())
      suffix += "containing \"";
    else
      suffix += "not containing \"";

    suffix += *m_lineEditText;

    suffix += "\"";
    return suffix;
  }


  void AbstractStringFilter::updateLineEditText(QString newText) {
    *m_lineEditText = newText;
  }
}
