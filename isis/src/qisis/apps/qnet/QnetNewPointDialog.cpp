#include "QnetNewPointDialog.h"

#include <algorithm>

#include <QLabel>
#include <QPushButton>
#include <QStringList>

#include "iString.h"
#include "qnet.h"
#include "SerialNumberList.h"

using namespace Qisis::Qnet;
using namespace Isis;

namespace Qisis {
  // initialize static variable
  QString QnetNewPointDialog::lastPtIdValue = "";

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
  QnetNewPointDialog::QnetNewPointDialog(QWidget *parent) : QDialog(parent) {

    ptIdValue = NULL;
    fileList = NULL;
    p_ptIdLabel = NULL;
    p_okButton = NULL;

    p_ptIdLabel = new QLabel("Point ID:");
    ptIdValue = new QLineEdit;
    p_ptIdLabel->setBuddy(ptIdValue);
    ptIdValue->setText(lastPtIdValue);
    ptIdValue->selectAll();
    connect(ptIdValue, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableOkButton(const QString &)));

    QLabel *listLabel = new QLabel("Select Files:");

    fileList = new QListWidget;
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);

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
    vLayout->addWidget(listLabel);
    vLayout->addWidget(fileList);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setWindowTitle("Create New ControlPoint");

  }

  void QnetNewPointDialog::SetFiles(QStringList pointFiles) {
  
    int bottomMostSelectedItemIndex = 0;
    
    for(int i = 0; i < g_serialNumberList->Size(); i++) {
      
      // build new item...
      iString label = g_serialNumberList->Filename(i);
      QListWidgetItem * item = new QListWidgetItem(label);
      
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
  void QnetNewPointDialog::enableOkButton(const QString &text) {
    QnetNewPointDialog::lastPtIdValue = ptIdValue->text();
    p_okButton->setEnabled(!text.isEmpty());
  }


}
