#include "QnetNewPointDialog.h"

#include <QtGui>
#include <algorithm>

#include "iString.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Isis::Qnet;

namespace Isis {
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
   *   @history 2011-03-08 Tracie Sucharski - If there is a saved ID
   *                          and there is no point in the network with that
   *                          id, do not disable "ok" button.  This allows
   *                          user to use the same id from a previous point
   *                          creation if the point was never saved.
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
    //  If the last point id used was never saved to network, do not set ok
    //  button to faslse
    if (lastPtIdValue.isEmpty() || g_controlNetwork->ContainsPoint(lastPtIdValue)) {
      p_okButton->setEnabled(false);
    }
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
   *   @history 2010-10-29 Tracie Sucharski - Changed std::vector<std::string>
   *            to QSringList
   */
  void QnetNewPointDialog::SetFiles(QStringList pointFiles) {

    int bottomMostSelectedItemIndex = 0;

    for (int i = 0; i < g_serialNumberList->Size(); i++) {

      // build new item...
      iString label = g_serialNumberList->Filename(i);
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
  void QnetNewPointDialog::enableOkButton(const QString &text) {
    QnetNewPointDialog::lastPtIdValue = ptIdValue->text();
    p_okButton->setEnabled(!text.isEmpty());
  }


}
