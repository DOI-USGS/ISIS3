#include "MosaicPanTool.h"

#include <iostream>

#include <QAction>
#include <QMenu>

#include "IString.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"

namespace Isis {
  /**
   * MosaicPanTool constructor
   *
   *
   * @param parent
   */
  MosaicPanTool::MosaicPanTool(MosaicSceneWidget *scene) : MosaicTool(scene) {
  }


  /**
   * Adds the action to the toolpad.
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *MosaicPanTool::getPrimaryAction() {
    QAction *action = new QAction(this);
    action->setIcon(getIcon("move.png"));
    //action->setIcon(QPixmap("/work1/salley/icons/mActionMoveVertex.png"));
    action->setToolTip("Pan (p)");
    action->setShortcut(Qt::Key_P);
    QString text  =
      "<b>Function:</b>  Pan around the current mosaic.<br><br>"
      "This tool gives you a <b>click and drag</b> to pan around the mosaic "
      "scene."
      "<p><b>Shortcut:</b>  p</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Creates the widget to add to the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *MosaicPanTool::createToolBarWidget() {
    QWidget *widget = new QWidget();
    return widget;
  }


  /**
   *
   *
   */
  void MosaicPanTool::updateTool() {
    if(isActive()) {
      getWidget()->setCubesSelectable(false);
      getWidget()->getView()->setDragMode(QGraphicsView::ScrollHandDrag);
    }
    else {
      getWidget()->setCubesSelectable(true);
      getWidget()->getView()->setDragMode(QGraphicsView::RubberBandDrag);
    }
  }

}

