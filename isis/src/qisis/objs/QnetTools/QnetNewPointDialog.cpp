#include "QnetNewPointDialog.h"

#include <algorithm>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QtWidgets>

#include "ControlNet.h"
#include "IString.h"
#include "QnetTool.h"
#include "SerialNumberList.h"

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
   *   @history 2011-03-08 Tracie Sucharski - If there is a saved ID
   *                          and there is no point in the network with that
   *                          id, do not disable "ok" button.  This allows
   *                          user to use the same id from a previous point
   *                          creation if the point was never saved.
   *
   */
  QnetNewPointDialog::QnetNewPointDialog(QnetTool *qnetTool, QString defaultPointId,
                                         QWidget *parent) : QDialog(parent) {

    m_ptIdEdit = NULL;
    m_fileList = NULL;
    m_ptIdLabel = NULL;
    m_okButton = NULL;

    m_qnetTool = qnetTool;

    m_ptIdLabel = new QLabel("Point ID:");
    m_ptIdEdit = new QLineEdit;
    m_ptIdLabel->setBuddy(m_ptIdEdit);
    m_ptIdEdit->setText(defaultPointId);
    m_ptIdEdit->selectAll();
    connect(m_ptIdEdit, SIGNAL(textChanged(const QString &)),
        this, SLOT(enableOkButton(const QString &)));

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
    vLayout->addWidget(listLabel);
    vLayout->addWidget(m_fileList);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setWindowTitle("Create New ControlPoint");

  }


  QString QnetNewPointDialog::pointId() const {
    return m_ptIdEdit->text();
  }


  QStringList QnetNewPointDialog::selectedFiles() const {
    QStringList result;

    foreach (QListWidgetItem *fileItem, m_fileList->selectedItems()) {
      result.append(fileItem->text());
    }

    return result;
  }


  /**
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::"
   *            since "using namespace std"
   *   @history 2010-10-29 Tracie Sucharski - Changed std::vector<std::string>
   *            to QSringList
   */
  void QnetNewPointDialog::setFiles(QStringList pointFiles) {

    int bottomMostSelectedItemIndex = 0;

    SerialNumberList *snList = m_qnetTool->serialNumberList();
    for (int i = 0; i < snList->size(); i++) {

      // build new item...
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
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *            to the ptIdValue
   */
  void QnetNewPointDialog::enableOkButton(const QString &) {
    m_okButton->setEnabled(!m_ptIdEdit->text().isEmpty() &&
                           !m_qnetTool->controlNet()->ContainsPoint(m_ptIdEdit->text()));
  }


}
