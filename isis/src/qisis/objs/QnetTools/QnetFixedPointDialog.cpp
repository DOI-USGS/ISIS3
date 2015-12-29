#include "QnetFixedPointDialog.h"

#include <QtGui>

#include <algorithm>

#include "SerialNumberList.h"
#include "ControlPoint.h"
#include "Camera.h"
#include "QnetTool.h"

using namespace std;

namespace Isis {
  /** 
   * QnetNewPointDialog constructor 
   * @param parent The parent widget for the
   *               cube points filter
   *  
   * @internal 
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *  
   */ 
  QnetFixedPointDialog::QnetFixedPointDialog(QnetTool *qnetTool, QString defaultPointId,
                                             QWidget *parent) : QDialog (parent) {
    m_qnetTool = qnetTool;
    
    m_avg = new QRadioButton("Average Measures");
    m_avg->setChecked(true);
    m_select = new QRadioButton("Select Measures");
    m_select->setChecked(false);
    //connect(m_avg,SIGNAL(clicked()),this,SLOT(selectMeasures()));
    //connect(m_select,SIGNAL(clicked()),this,SLOT(selectMeasures()));
    m_ptIdValue = NULL;
    m_fileList = NULL;
    m_ptIdLabel = NULL;
    m_okButton = NULL;
    m_pointFiles = NULL;

    m_ptIdLabel = new QLabel("Point ID:");
    m_ptIdValue = new QLineEdit;
    m_ptIdLabel->setBuddy(m_ptIdValue);
    m_ptIdValue->setText(defaultPointId);
    m_ptIdValue->selectAll();
    connect(m_ptIdValue,SIGNAL(textChanged(const QString &)),
            this,SLOT(enableOkButton(const QString &)));

    QGroupBox *pointTypeGroup = new QGroupBox("Point Type");
    m_fixed = new QRadioButton("Fixed");
    m_constrained = new QRadioButton("Constrained");
    m_constrained->setChecked(true);
    QVBoxLayout *pointTypeLayout = new QVBoxLayout;
    pointTypeLayout->addWidget(m_fixed);
    pointTypeLayout->addWidget(m_constrained);
    pointTypeGroup->setLayout(pointTypeLayout);

    QLabel *listLabel = new QLabel("Select Files:");

    m_fileList = new QListWidget();
    m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //m_fileList->setEnabled(false);

    //  Create OK & Cancel buttons
    m_okButton = new QPushButton("OK");
    m_okButton->setEnabled(false);
    QPushButton *cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(cancelButton);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *ptIdLayout = new QHBoxLayout;
    ptIdLayout->addWidget(m_ptIdLabel);
    ptIdLayout->addWidget(m_ptIdValue);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(ptIdLayout);
    vLayout->addWidget(pointTypeGroup);
    vLayout->addWidget(listLabel);
    vLayout->addWidget(m_fileList);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setWindowTitle("Create Fixed or Constrained ControlPoint");

  }

  
  QnetFixedPointDialog::~QnetFixedPointDialog() {
    delete m_pointFiles;
    m_pointFiles = NULL;
  }


  bool QnetFixedPointDialog::isFixed() const {
    return m_fixed->isChecked();
  }


  bool QnetFixedPointDialog::isConstrained() const {
    return m_constrained->isChecked();
  }


  QString QnetFixedPointDialog::pointId() const {
    return m_ptIdValue->text();
  }


  QStringList QnetFixedPointDialog::selectedFiles() const {
    QStringList result;

    foreach (QListWidgetItem *fileItem, m_fileList->selectedItems()) {
      result.append(fileItem->text());
    }

    return result;
  }


  /** 
   * Set files found containing selected point 
   *  
   * @author 2010-11-10 Tracie Sucharski 
   *  
   * @internal 
   * @history 2011-09-16 Tracie Sucharski - List images that insersect 
   *                        point at the top of the list. 
   */
  void QnetFixedPointDialog::setFiles (QStringList pointFiles) {
    m_pointFiles = new QStringList(pointFiles);

    int bottomMostSelectedItemIndex = 0;

    //  Add all files to list , selecting those in pointFiles which are
    //  those files which contain the point.
    SerialNumberList *snList = m_qnetTool->serialNumberList();
    for (int i = 0; i < snList->size(); i++) {
      QString label = snList->fileName(i);
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
   * @param text 
   * @internal 
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *            to the m_ptIdValue 
   */
  void QnetFixedPointDialog::enableOkButton (const QString &text) {
    m_okButton->setEnabled(!text.isEmpty());
  }
}
