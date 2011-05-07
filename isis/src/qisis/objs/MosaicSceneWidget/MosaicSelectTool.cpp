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
      "<b>Function:</b>  View different areas of the mosaic. \
      <p><b>Shortcut:</b>  S</p> ";
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

