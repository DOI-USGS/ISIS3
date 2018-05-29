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
   * @description Create dialog for creating a new Control Point
   *  
   * @param controlNet               The control net the new control point will be contained in 
   * @param serialNumberList         The serial number list corresponding to the controlNet 
   * @param defaultPointId           The default pointID, usually empty string 
   * @param parent                   Parent widget 
   * @param pointType                Show the Point Type combo box, default = false 
   * @param groundSource             Show the Ground Source list, default = false 
   * @param subpixelRegisterMeasures Show the check box for sub-pixel registration option, default = false
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
      m_pointTypeCombo->setCurrentText("Free");
      QLabel *pointTypeLabel = new QLabel("Point Type:");
      pointTypeLayout->addWidget(pointTypeLabel);
      pointTypeLayout->addWidget(m_pointTypeCombo);
      connect(m_pointTypeCombo, SIGNAL(currentIndexChanged(QString)),
              this, SLOT(pointTypeChanged(QString)));
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
    int result = ControlPoint::Free;
    if (m_pointTypeCombo->currentText() == "Constrained") {
      result = ControlPoint::Constrained;
    }
    if (m_pointTypeCombo->currentText() == "Fixed") {
      result = ControlPoint::Fixed;
    }
    return result;
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


  void NewControlPointDialog::pointTypeChanged(QString pointType) {
    if (pointType == "Fixed" || pointType == "Constrained") {
      m_groundSourceCombo->setVisible(true);
    }
  }


  void NewControlPointDialog::setGroundSource(QStringList groundFiles, int numberShapesWithPoint) {
    //  If groundFiles not empty, add to the list widget for selection
    if (groundFiles.count() != 0) {
      m_groundSourceCombo->addItems(groundFiles); 
      for (int i = 0; i < numberShapesWithPoint; i++) {
        m_groundSourceCombo->setItemData(i, QColor(Qt::red), Qt::ForegroundRole);
      }
      m_groundSourceCombo->insertSeparator(numberShapesWithPoint);
    }
    // If groundFiles is empty, remove option to change point type to Constrained or Fixed, add a
    // tooltip to give user hint as to why they don't have option to change point type and set
    // default point type back to "Free".
    else {
      m_pointTypeCombo->setToolTip("The Point Type cannot be changed to \"Fixed\" or "
                                   "\"Constrained\", because there are no shapes imported into "
                                   "your project.");
      m_pointTypeCombo->removeItem(m_pointTypeCombo->findText("Constrained"));
      m_pointTypeCombo->removeItem(m_pointTypeCombo->findText("Fixed"));
      m_pointTypeCombo->setCurrentText("Free");
    }
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
    bool enable = !m_ptIdEdit->text().isEmpty() &&
                  !m_controlNet->ContainsPoint(m_ptIdEdit->text());
    m_okButton->setEnabled(enable);
    if (enable) {
      m_okButton->setToolTip("");
    }
    else {
      m_okButton->setToolTip("Cannot create point because Point Id is either empty or the active "
                             "control net already contains a control point with this point Id.");
    }
  }
}
