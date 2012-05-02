#include "QnetNewMeasureDialog.h"

#include <algorithm>
#include <string>

#include <QtGui>

#include "ControlPoint.h"
#include "iString.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Isis::Qnet;

namespace Isis {
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
   *   @history 2010-10-29 Tracie Sucharski - Changed std::vector<std::string>
   *            to QSringList
   */
  void QnetNewMeasureDialog::SetFiles(ControlPoint point,
      QStringList pointFiles) {
    int bottomMostSelectedItemIndex = 0;

    //  Add all entries in the SerialNumberList
    for (int i = 0; i < g_serialNumberList->Size(); i++) {

      iString curSerialNum = g_serialNumberList->SerialNumber(i);

      //  Don't add if already in this point
      if (point.HasSerialNumber(curSerialNum))
        continue;

      // build new item...
      iString label(g_serialNumberList->FileName(i));
      QListWidgetItem *item = new QListWidgetItem(label);

      // if this entry of the SerialNumberList is also in the pointFiles then
      // mark it as selected and insert after the last selected item (toward
      // the top, otherwise add it to the end
      if (pointFiles.contains(label)) {
        fileList->insertItem(bottomMostSelectedItemIndex++, item);
        item->setSelected(true);
      }
      else {
        fileList->addItem(item);
      }
    }
  }


  void QnetNewMeasureDialog::enableOkButton(const QString &text) {
    p_okButton->setEnabled(!text.isEmpty());
  }

}
