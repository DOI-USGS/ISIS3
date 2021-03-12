/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CnetEditorSortConfigDialog.h"

#include <limits>

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QtWidgets>

#include "AbstractTableModel.h"
#include "CnetEditorWidget.h"

namespace Isis {
  /**
   * Create a config dialog that configures the given FeatureCnetEditorSort.
   *
   * @param tool The tool to read settings from and write settings to.
   * @param parent The qt-parent relationship parent.
   */
  CnetEditorSortConfigDialog::CnetEditorSortConfigDialog(
    CnetEditorWidget *cnetWidget) : QDialog(cnetWidget) {
    m_cnetWidget = cnetWidget;

    setWindowTitle("Table Sorting");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnMinimumWidth(0, 20);
    setLayout(mainLayout);

    // Settings area
    int row = 0;
    QLabel *pointTableLabel = new QLabel("<h3>Point Table</h3>");
    mainLayout->addWidget(pointTableLabel, row, 0, 1, 3);
    row++;

    QLabel *pointSortEnableLabel = new QLabel("Sorting Enabled");
    mainLayout->addWidget(pointSortEnableLabel, row, 1);

    m_pointSortingCheckBox = new QCheckBox;
    mainLayout->addWidget(m_pointSortingCheckBox, row, 2);
    connect(m_pointSortingCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(refreshWidgetStates()));
    row++;

    QLabel *pointLimitLabel = new QLabel("Table Size Limit");
    mainLayout->addWidget(pointLimitLabel, row, 1);

    m_pointTableLimitSpinBox = new QSpinBox;
    m_pointTableLimitSpinBox->setRange(2, std::numeric_limits<int>::max());
    mainLayout->addWidget(m_pointTableLimitSpinBox, row, 2);
    row++;

    m_pointTableWarningsLabel = new QLabel;
    m_pointTableWarningsLabel->setVisible(false);
    m_pointTableWarningsLabel->setWordWrap(true);
    mainLayout->addWidget(m_pointTableWarningsLabel, row, 1, 1, 2);
    row++;

    QLabel *measureTableLabel = new QLabel("<h3>Measure Table</h3>");
    mainLayout->addWidget(measureTableLabel, row, 0, 1, 3);
    row++;

    QLabel *measureSortEnableLabel = new QLabel("Sorting Enabled");
    mainLayout->addWidget(measureSortEnableLabel, row, 1);

    m_measureSortingCheckBox = new QCheckBox;
    mainLayout->addWidget(m_measureSortingCheckBox, row, 2);
    connect(m_measureSortingCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(refreshWidgetStates()));
    row++;

    QLabel *measureLimitLabel = new QLabel("Table Size Limit");
    mainLayout->addWidget(measureLimitLabel, row, 1);

    m_measureTableLimitSpinBox = new QSpinBox;
    m_measureTableLimitSpinBox->setRange(2, std::numeric_limits<int>::max());
    mainLayout->addWidget(m_measureTableLimitSpinBox, row, 2);
    row++;

    m_measureTableWarningsLabel = new QLabel;
    m_measureTableWarningsLabel->setVisible(false);
    m_measureTableWarningsLabel->setWordWrap(true);
    mainLayout->addWidget(m_measureTableWarningsLabel, row, 1, 1, 2);
    row++;

    // Now the buttons area
    QHBoxLayout *buttonsAreaLayout = new QHBoxLayout;

    buttonsAreaLayout->addStretch();

    QPushButton *okayButton = new QPushButton("&Ok");
    okayButton->setIcon(QIcon::fromTheme("dialog-ok"));
    buttonsAreaLayout->addWidget(okayButton);
    connect(okayButton, SIGNAL(clicked()),
        this, SLOT(applySettings()));
    connect(okayButton, SIGNAL(clicked()),
        this, SLOT(accept()));

    QPushButton *applyButton = new QPushButton("&Apply");
    applyButton->setIcon(QIcon::fromTheme("dialog-ok-apply"));
    buttonsAreaLayout->addWidget(applyButton);
    connect(applyButton, SIGNAL(clicked()),
        this, SLOT(applySettings()));

    QPushButton *cancelButton = new QPushButton("&Cancel");
    cancelButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    buttonsAreaLayout->addWidget(cancelButton);
    connect(cancelButton, SIGNAL(clicked()),
        this, SLOT(reject()));

    QWidget *buttonsAreaWidget = new QWidget;
    buttonsAreaWidget->setLayout(buttonsAreaLayout);
    mainLayout->addWidget(buttonsAreaWidget, row, 0, 1, 3);

    readSettings();
    refreshWidgetStates();
  }

  /**
   * Clean up allocated memory.
   */
  CnetEditorSortConfigDialog::~CnetEditorSortConfigDialog() {
  }


  /**
   * Apply the user's current settings to the cneteditor widget.
   */
  void CnetEditorSortConfigDialog::applySettings() {
    m_cnetWidget->setPointTableSortingEnabled(
      m_pointSortingCheckBox->isChecked());
    m_cnetWidget->setPointTableSortLimit(
      m_pointTableLimitSpinBox->value());

    m_cnetWidget->setMeasureTableSortingEnabled(
      m_measureSortingCheckBox->isChecked());
    m_cnetWidget->setMeasureTableSortLimit(
      m_measureTableLimitSpinBox->value());

    readSettings();
  }


  /**
   * Read the cneteditor widget's current settings and set the widget states to
   * match.
   */
  void CnetEditorSortConfigDialog::readSettings() {
    // Point Table
    m_pointSortingCheckBox->setChecked(
      m_cnetWidget->pointTableSortingEnabled());
    m_pointTableLimitSpinBox->setValue(m_cnetWidget->pointTableSortLimit());

    AbstractTableModel *pointModel = m_cnetWidget->pointTableModel();
    QString disabledWarning = tr("<font color='red'>Sorting is currently disabled because the "
        "number of visible rows (%L1) exceeds the applied table size limit option (%L2).</font>");

    if (pointModel && m_pointSortingCheckBox->isChecked() &&
        pointModel->sortLimit() < pointModel->getVisibleRowCount()) {
      m_pointTableWarningsLabel->setText(
        disabledWarning.arg(pointModel->getVisibleRowCount()).arg(pointModel->sortLimit()));

      m_pointTableWarningsLabel->setVisible(true);
    }
    else {
      m_pointTableWarningsLabel->setText(tr(""));
      m_pointTableWarningsLabel->setVisible(false);
    }

    // Measure Table
    m_measureSortingCheckBox->setChecked(
      m_cnetWidget->measureTableSortingEnabled());
    m_measureTableLimitSpinBox->setValue(m_cnetWidget->measureTableSortLimit());

    AbstractTableModel *measureModel = m_cnetWidget->measureTableModel();
    if (measureModel && m_measureSortingCheckBox->isChecked() &&
        measureModel->sortLimit() < measureModel->getVisibleRowCount()) {
      m_measureTableWarningsLabel->setText(
        disabledWarning.arg(measureModel->getVisibleRowCount()).arg(measureModel->sortLimit()));

      m_measureTableWarningsLabel->setVisible(true);
    }
    else {
      m_measureTableWarningsLabel->setText(tr(""));
      m_measureTableWarningsLabel->setVisible(false);
    }

    // Resize the dialog (for when warnings come and go, for example).
    adjustSize();
  }


  /**
   * Enable or disable inputs based on what the user has selected for options
   * so far.
   */
  void CnetEditorSortConfigDialog::refreshWidgetStates() {
    m_pointTableLimitSpinBox->setEnabled(m_pointSortingCheckBox->isChecked());
    m_measureTableLimitSpinBox->setEnabled(
      m_measureSortingCheckBox->isChecked());
  }
}
