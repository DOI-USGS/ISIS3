#include "QnetNewMeasureDialog.h"

#include <algorithm>
#include <string>

#include <QtGui>

#include "ControlPoint.h"
#include "IString.h"
#include "QnetTool.h"
#include "SerialNumberList.h"

namespace Isis {
  /**
   * Contructor.
   *
   * @param parent The parent widget for the cube points filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *
   */
  QnetNewMeasureDialog::QnetNewMeasureDialog(QnetTool *qnetTool,
                                             QWidget *parent) : QDialog(parent) {
    m_fileList = NULL;
    m_okButton = NULL;
    m_qnetTool = qnetTool;

    QLabel *listLabel = new QLabel("Select Files:");

    m_fileList = new QListWidget;
    m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    //  Create OK & Cancel buttons
    m_okButton = new QPushButton("OK");
    //m_okButton->setEnabled(false);
    QPushButton *cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(cancelButton);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(listLabel);
    vLayout->addWidget(m_fileList);
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
  void QnetNewMeasureDialog::setFiles(ControlPoint point,
      QStringList pointFiles) {
    int bottomMostSelectedItemIndex = 0;

    //  Add all entries in the SerialNumberList
    SerialNumberList *snList = m_qnetTool->serialNumberList();
    for (int i = 0; i < snList->Size(); i++) {
      QString curSerialNum = snList->SerialNumber(i);

      //  Don't add if already in this point
      if (point.HasSerialNumber(curSerialNum))
        continue;

      // build new item...
      QString label(snList->FileName(i));
      QListWidgetItem *item = new QListWidgetItem(label);

      // if this entry of the SerialNumberList is also in the pointFiles then
      // mark it as selected and insert after the last selected item (toward
      // the top, otherwise add it to the end
      if (pointFiles.contains(label)) {
        m_fileList->insertItem(bottomMostSelectedItemIndex++, item);
        item->setSelected(true);
      }
      else {
        m_fileList->addItem(item);
      }
    }
  }


  QStringList QnetNewMeasureDialog::selectedFiles() const {
    QStringList result;

    foreach (QListWidgetItem *fileItem, m_fileList->selectedItems()) {
      result.append(fileItem->text());
    }

    return result;
  }


  void QnetNewMeasureDialog::enableOkButton(const QString &text) {
    m_okButton->setEnabled(!text.isEmpty());
  }
}
