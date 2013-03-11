#include "WarningTreeWidget.h"

namespace Isis {
  WarningTreeWidget::WarningTreeWidget(QWidget *parent) : QTreeWidget(parent) {

    setHeaderHidden(true);
    //setHeaderLabel("");
  }



  WarningTreeWidget::~WarningTreeWidget() {

  }


  void WarningTreeWidget::showWarning(QString warningText) {
    QStringList columnData;
    columnData.append(warningText);

    QTreeWidgetItem *newItem = new QTreeWidgetItem(columnData);
    invisibleRootItem()->addChild(newItem);
  }
}
