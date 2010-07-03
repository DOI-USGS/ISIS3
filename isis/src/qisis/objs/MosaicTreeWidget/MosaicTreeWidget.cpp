#include <QTreeWidget>

#include "MosaicTreeWidget.h"
#include "MosaicMainWindow.h"

namespace Qisis {

  /**
   * MosaicTreeWidget constructor. 
   * MosaicTreeWidget is derived from QTreeWidget
   * 
   * 
   * @param parent 
   */
  MosaicTreeWidget::MosaicTreeWidget (QWidget *parent) : QTreeWidget(parent) {

  }

  /**
   * This is why we needed to subclass the QTreeWidget class.
   * We needed our own dropEvent for the dragging and dropping
   * of the tree widget items.
   */
  void MosaicTreeWidget::dropEvent(QDropEvent *event) {
    emit itemDropped(event->pos());
  }

}
