#include <QtGui>
#include <QString>

#include <algorithm>

#include "QnetNewPointDialog.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Qisis::Qnet;
using namespace std;

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
    p_pointFiles = NULL;

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

  /**
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::"
   *            since "using namespace std"
   */
  void QnetNewPointDialog::SetFiles(vector<string> &pointFiles) {
    //  TODO::  make pointFiles const???
    p_pointFiles = &pointFiles;

    //  Add all files to list , selecting those in pointFiles which are
    //  those files which contain the point.
    for(int i = 0; i < g_serialNumberList->Size(); i++) {
      QListWidgetItem *item = new QListWidgetItem(fileList);
      string tempFilename = g_serialNumberList->Filename(i);
      item->setText(QString(tempFilename.c_str()));
      vector<string>::iterator pos;
      pos = std::find(p_pointFiles->begin(), p_pointFiles->end(),
                      g_serialNumberList->Filename(i));
      if(pos != p_pointFiles->end()) {
        fileList->setItemSelected(item, true);
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
