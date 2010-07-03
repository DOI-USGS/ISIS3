#include <QtGui>

#include <algorithm>

#include "QnetNewMeasureDialog.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Qisis::Qnet;
using namespace std;

namespace Qisis {
  /**
   * Contructor.
   *
   * @param parent The parent widget for the
   *               cube points filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *
   */
  QnetNewMeasureDialog::QnetNewMeasureDialog(QWidget *parent) : QDialog(parent) {
    fileList = NULL;
    p_okButton = NULL;
    p_pointFiles = NULL;


    QLabel *listLabel = new QLabel("Select Files:");

    fileList = new QListWidget;
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    //  Create OK & Cancel buttons
    p_okButton = new QPushButton("OK");
    //p_okButton->setEnabled(false);
    QPushButton *cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(p_okButton);
    buttonLayout->addWidget(cancelButton);

    connect(p_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(listLabel);
    vLayout->addWidget(fileList);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setWindowTitle("Add Measures to ControlPoint");

  }


  /**
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::"
   *            since "using namespace std"
   */
  void QnetNewMeasureDialog::SetFiles(const Isis::ControlPoint &point,
                                      vector<string> &pointFiles) {
    //  TODO::  make pointFiles const???
    p_pointFiles = &pointFiles;
    Isis::ControlPoint pt = point;

    //  Add all files to list , selecting those in pointFiles which are
    //  those files which contain the point.
    for(int i = 0; i < g_serialNumberList->Size(); i++) {
      //  Don't add if already in this point
      string sn = g_serialNumberList->SerialNumber(i);
      if(pt.HasSerialNumber(sn)) continue;
      //if (point.HasSN(sn)) continue;

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


  void QnetNewMeasureDialog::enableOkButton(const QString &text) {
    p_okButton->setEnabled(!text.isEmpty());
  }


}
