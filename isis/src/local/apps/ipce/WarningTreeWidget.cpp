#include "WarningTreeWidget.h"

#include <QtDebug>

#include <QTreeWidgetItem>

namespace Isis {
  /**
   * Create a warning tree widget for the given project.
   *
   * @param parent The widget that goes along with the warnings.
   */
  WarningTreeWidget::WarningTreeWidget(QWidget *parent) : QTreeWidget(parent) {

    setHeaderHidden(true);
  }


  WarningTreeWidget::~WarningTreeWidget() {

  }


  /**
   * Creates a new warning item with the given text.
   *
   * @param text The text of the warning.
   */
  void WarningTreeWidget::showWarning(QString text) {

    QTreeWidgetItem *warningItem = new QTreeWidgetItem(this);
    warningItem->setText(0, text);
  }
}
