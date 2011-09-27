#include "MosaicSelectTool.h"

#include <iostream>

#include <QMenu>

#include "iString.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"

namespace Isis {
  /**
   * MosaicSelectTool constructor
   *
   *
   * @param parent
   */
  MosaicSelectTool::MosaicSelectTool(MosaicSceneWidget *scene) :
      MosaicTool(scene) {
  }


  /**
   * Adds the action to the toolpad.
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *MosaicSelectTool::getPrimaryAction() {
    QAction *action = new QAction(this);
    action->setIcon(getIcon("mActionSelect.png"));
    action->setToolTip("Select (S)");
    action->setShortcut(Qt::Key_S);
    QString text  =
      "<b>Function:</b>  Select cubes in a mosaic.<br><br>"
      "This tool gives you a <b>drag-select</b> to select multiple files (this "
      "selects files underneath the top one), a <b>control-click</b> select to "
      "add files to the current selection, and a <b>click</b> selection to "
      "replace the current selection with the file you clicked on."
      "<br><p><b>Shortcut:</b>  S</p>";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * This method sets the QGraphicsView to allow the user to select
   * mosaic items by dragging a rubber band.
   *
   */
  void MosaicSelectTool::updateTool() {
    if(isActive()) {
      getWidget()->getView()->setDragMode(QGraphicsView::RubberBandDrag);
    }
    else {
      getWidget()->getView()->setDragMode(QGraphicsView::NoDrag);
    }
  }

}

