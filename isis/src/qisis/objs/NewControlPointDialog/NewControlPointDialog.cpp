#include "NewControlPointDialog.h"

#include <algorithm>

#include <QComboBox>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QtDebug>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "IString.h"
#include "SerialNumberList.h"

namespace Isis {
  /**
   * NewControlPointDialog constructor
   * @param parent The parent widget for the
   *               cube points filter
   *
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *   @history 2011-03-08 Tracie Sucharski - If there is a saved ID
   *                          and there is no point in the network with that
   *                          id, do not disable "ok" button.  This allows
   *                          user to use the same id from a previous point
   *                          creation if the point was never saved.
   *
   */
  NewControlPointDialog::NewControlPointDialog(ControlNet *controlNet,
                                               SerialNumberList *serialNumberList,
                                               QString defaultPointId,
                                               QWidget *parent,
                                               bool pointType,
                                               bool groundSource,
                                               bool subpixelRegisterMeasures) : QDialog(parent) {

    m_controlNet = controlNet;
    m_serialNumberList = serialNumberList;

    m_ptIdEdit = NULL;

    m_subpixelRegisterButton = NULL;
    m_fileList = NULL;
    m_ptIdLabel = NULL;
    m_okButton = NULL;

    m_ptIdLabel = new QLabel("Point ID:");
    m_ptIdEdit = new QLineEdit;
    m_ptIdLabel->setBuddy(m_ptIdEdit);
    m_ptIdEdit->setText(defaultPointId);
    m_ptIdEdit->selectAll();
    connect(m_ptIdEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableOkButton(const QString &)));

    QHBoxLayout *pointTypeLayout = new QHBoxLayout();
    if (pointType) {
      m_pointTypeCombo = new QComboBox;
      for (int i=0; i<ControlPoint::PointTypeCount; i++) {
        m_pointTypeCombo->insertItem(i, ControlPoint::PointTypeToString(
                                     (ControlPoint::PointType) i));
      }
      m_pointTypeCombo->setCurrentIndex(2);
      QLabel *pointTypeLabel = new QLabel("PointType:");
      pointTypeLayout->addWidget(pointTypeLabel);
      pointTypeLayout->addWidget(m_pointTypeCombo);
      connect(m_pointTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(pointTypeChanged(int)));
    }


    if (groundSource) {
      m_groundSourceLayout = new QHBoxLayout();
      m_groundSourceCombo = new QComboBox;
      QLabel *groundSourceLabel = new QLabel("Ground Source:");
      m_groundSourceLayout->addWidget(groundSourceLabel);
      m_groundSourceLayout->addWidget(m_groundSourceCombo);
      m_groundSourceCombo->setVisible(false);
    }

    if (subpixelRegisterMeasures) {
      m_subpixelRegisterButton = new QRadioButton("Subpixel Register Measures");
      m_subpixelRegisterButton->setChecked(true);
      m_subpixelRegisterButton->setToolTip("Each measure will be subpixel registered to the reference"
                                           " as it is created.");
    }

    QLabel *listLabel = new QLabel("Select Files:");

    m_fileList = new QListWidget;
    m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    //  Create OK & Cancel buttons
    m_okButton = new QPushButton("OK");
    //  If the last point id used was never saved to network, do not set ok
    //  button to faslse
    enableOkButton("");

    QPushButton *cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(cancelButton);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *ptIdLayout = new QHBoxLayout;
    ptIdLayout->addWidget(m_ptIdLabel);
    ptIdLayout->addWidget(m_ptIdEdit);

    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->addLayout(ptIdLayout);

    if (pointType) {
      vLayout->addLayout(pointTypeLayout);
    }

    if (groundSource) {
      vLayout->addLayout(m_groundSourceLayout);
    }

    if (subpixelRegisterMeasures) {
      vLayout->addWidget(m_subpixelRegisterButton);
    }

    vLayout->addWidget(listLabel);
    vLayout->addWidget(m_fileList);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setWindowTitle("Create New ControlPoint");

  }


  QString NewControlPointDialog::pointId() const {
    return m_ptIdEdit->text();
  }


  int NewControlPointDialog::pointType() const {
    return m_pointTypeCombo->currentIndex();
  }


  QStringList NewControlPointDialog::selectedFiles() const {
    QStringList result;

    foreach (QListWidgetItem *fileItem, m_fileList->selectedItems()) {
      result.append(fileItem->text());
    }

    return result;
  }


  bool NewControlPointDialog::subpixelRegisterPoint() {
    return m_subpixelRegisterButton->isChecked();
  }


  QString NewControlPointDialog::groundSource() const {
    return m_groundSourceCombo->currentText();
  }


  void NewControlPointDialog::pointTypeChanged(int pointType) {
    if (pointType == ControlPoint::Constrained || pointType == ControlPoint::Fixed) {
      m_groundSourceCombo->setVisible(true);
    }
  }


  void NewControlPointDialog::setGroundSource(QStringList groundFiles, int numberShapesWithPoint) {
    m_groundSourceCombo->addItems(groundFiles);
    for (int i = 0; i < numberShapesWithPoint; i++) {
      m_groundSourceCombo->setItemData(i, QColor(Qt::red), Qt::ForegroundRole);
    }
    m_groundSourceCombo->insertSeparator(numberShapesWithPoint);
  }


  /**
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::"
   *            since "using namespace std"
   *   @history 2010-10-29 Tracie Sucharski - Changed std::vector<std::string>
   *            to QSringList
   */
  void NewControlPointDialog::setFiles(QStringList pointFiles) {

    int bottomMostSelectedItemIndex = 0;

    for (int i = 0; i < m_serialNumberList->size(); i++) {

      // build new item...
      QString label = m_serialNumberList->fileName(i);
      QListWidgetItem *item = new QListWidgetItem(label);

      // if this entry of the SerialNumberList is also in the pointFiles then
      // mark it as selected and insert after the last selected item (toward
      // the top of the list).  Otherwise just add the item to the end of the
      // list
      if (pointFiles.contains(label)) {
        m_fileList->insertItem(bottomMostSelectedItemIndex++, item);
        item->setSelected(true);
      }
      else {
        m_fileList->addItem(item);
      }
    }
  }


  /**
   *
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *            to the ptIdValue
   */
  void NewControlPointDialog::enableOkButton(const QString &) {
    m_okButton->setEnabled(!m_ptIdEdit->text().isEmpty() &&
                           !m_controlNet->ContainsPoint(m_ptIdEdit->text()));
  }
}
