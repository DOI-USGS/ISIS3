#include "QnetFixedPointDialog.h"

#include <QtGui>

#include <algorithm>

#include "SerialNumberList.h"
#include "ControlPoint.h"
#include "Camera.h"

#include "qnet.h"

using namespace Isis::Qnet;
using namespace std;

namespace Isis {
  // initialize static variable
  QString QnetFixedPointDialog::lastPtIdValue = "";

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
  QnetFixedPointDialog::QnetFixedPointDialog (QWidget *parent) : QDialog (parent) {

    
    p_avg = new QRadioButton("Average Measures");
    p_avg->setChecked(true);
    p_select = new QRadioButton("Select Measures");
    p_select->setChecked(false);
    //connect(p_avg,SIGNAL(clicked()),this,SLOT(selectMeasures()));
    //connect(p_select,SIGNAL(clicked()),this,SLOT(selectMeasures()));
    ptIdValue = NULL;
    fileList = NULL;
    p_ptIdLabel = NULL;
    p_okButton = NULL;
    p_pointFiles = NULL;

    p_ptIdLabel = new QLabel("Point ID:");
    ptIdValue = new QLineEdit;
    p_ptIdLabel->setBuddy(ptIdValue);
    ptIdValue->setText(lastPtIdValue);
    ptIdValue->selectAll();
    connect(ptIdValue,SIGNAL(textChanged(const QString &)),
            this,SLOT(enableOkButton(const QString &)));

    QGroupBox *pointTypeGroup = new QGroupBox("Point Type");
    fixed = new QRadioButton("Fixed");
    constrained = new QRadioButton("Constrained");
    constrained->setChecked(true);
    QVBoxLayout *pointTypeLayout = new QVBoxLayout;
    pointTypeLayout->addWidget(fixed);
    pointTypeLayout->addWidget(constrained);
    pointTypeGroup->setLayout(pointTypeLayout);

    QLabel *listLabel = new QLabel("Select Files:");

    fileList = new QListWidget();
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //fileList->setEnabled(false);

    //  Create OK & Cancel buttons
    p_okButton = new QPushButton("OK");
    p_okButton->setEnabled(false);
    QPushButton *cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(p_okButton);
    buttonLayout->addWidget(cancelButton);

    connect(p_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *ptIdLayout = new QHBoxLayout;
    ptIdLayout->addWidget(p_ptIdLabel);
    ptIdLayout->addWidget(ptIdValue);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(ptIdLayout);
    vLayout->addWidget(pointTypeGroup);
    vLayout->addWidget(listLabel);
    vLayout->addWidget(fileList);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setWindowTitle("Create Fixed or Constrained ControlPoint");

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
  void QnetFixedPointDialog::SetFiles (QStringList &pointFiles) {
    //  TODO::  make pointFiles const???
    p_pointFiles = &pointFiles;

    int bottomMostSelectedItemIndex = 0;

    //  Add all files to list , selecting those in pointFiles which are
    //  those files which contain the point.
    for (int i=0; i<g_serialNumberList->Size(); i++) {
      iString label = g_serialNumberList->FileName(i);
      QListWidgetItem *item = new QListWidgetItem(label);

      // if this entry of the SerialNumberList is also in the pointFiles then
      // mark it as selected and insert after the last selected item (toward
      // the top of the list).  Otherwise just add the item to the end of the
      // list
      if (pointFiles.contains(label)) {
        fileList->insertItem(bottomMostSelectedItemIndex++, item);
        item->setSelected(true);
      }
      else {
        fileList->addItem(item);
      }
    }
  }


  /** 
   *  
   * @param text 
   * @internal 
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *            to the p_ptIdValue 
   */
  void QnetFixedPointDialog::enableOkButton (const QString &text) {
    QnetFixedPointDialog::lastPtIdValue = ptIdValue->text();
    p_okButton->setEnabled(!text.isEmpty());
  }
}
