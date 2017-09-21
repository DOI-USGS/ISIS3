#include "IsisDebug.h"

#include "AbstractMultipleChoiceFilter.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QMargins>
#include <QString>
#include <QStringList>


namespace Isis {
  AbstractMultipleChoiceFilter::AbstractMultipleChoiceFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
    nullify();
    m_curChoice = new QString;
  }

  AbstractMultipleChoiceFilter::AbstractMultipleChoiceFilter(
    AbstractMultipleChoiceFilter const &other)
    : AbstractFilter(other) {
    nullify();
    m_curChoice = new QString;

    QStringList options;
    for (int i = 0; i < other.m_combo->count(); i++)
      options.append(other.m_combo->itemText(i));

    createWidget(options);

    m_combo->setCurrentIndex(other.m_combo->currentIndex());
  }



  AbstractMultipleChoiceFilter::~AbstractMultipleChoiceFilter() {
    if (m_curChoice) {
      delete m_curChoice;
      m_curChoice = NULL;
    }
  }


  void AbstractMultipleChoiceFilter::nullify() {
    m_combo = NULL;
    m_curChoice = NULL;
  }


  void AbstractMultipleChoiceFilter::createWidget(QStringList options) {
    m_combo = new QComboBox;
    foreach (QString option, options)
      m_combo->addItem(option);

    m_combo->setCurrentIndex(0);

    *m_curChoice = m_combo->currentText();

    connect(m_combo, SIGNAL(currentIndexChanged(QString)),
        this, SLOT(updateCurChoice(QString)));

    QHBoxLayout *layout = new QHBoxLayout;
    QMargins margins = layout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    layout->setContentsMargins(margins);
    layout->addWidget(m_combo);
    layout->addStretch();

    getMainLayout()->addLayout(layout);
  }


  QString const &AbstractMultipleChoiceFilter::getCurrentChoice() const {
    ASSERT(m_curChoice);
    return *m_curChoice;
  }


  void AbstractMultipleChoiceFilter::updateCurChoice(QString newChoice) {
    ASSERT(m_curChoice);

    *m_curChoice = newChoice;

    emit filterChanged();
  }
}
