#include "MosaicSelectTool.h"

#include <QMenu>

namespace Qisis {
  /**
   * MosaicSelectTool constructor
   * 
   * 
   * @param parent 
   */
  MosaicSelectTool::MosaicSelectTool (QWidget *parent) : Qisis::MosaicTool(parent) {
    connect(this,SIGNAL(activated(bool)),this,SLOT(updateTool()));
  }


  /**
   * Adds the action to the toolpad.
   * 
   * 
   * @param toolpad 
   * 
   * @return QAction* 
   */
  QAction *MosaicSelectTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir()+"/mActionSelect.png"));
    action->setToolTip("Select (S)");
    action->setShortcut(Qt::Key_S);
    QString text  =
      "<b>Function:</b>  View different areas of the mosaic. \
      <p><b>Shortcut:</b>  S</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Adds the pan action to the given menu.
   * 
   * 
   * @param menu 
   */
  void MosaicSelectTool::addToMenu(QMenu *menu) {
    
  }


  /**
   * Creates the widget to add to the tool bar.
   * 
   * 
   * @param parent 
   * 
   * @return QWidget* 
   */
  QWidget *MosaicSelectTool::createToolBarWidget (QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);
    return hbox;
  }

 

 /** 
  * This method sets the QGraphicsView to allow the user to select
  * mosaic items by dragging a rubber band.
  * 
  */
  void MosaicSelectTool::updateTool() {
    if(isActive()) {
      getGraphicsView()->setDragMode(QGraphicsView::RubberBandDrag);
    } else {
      getGraphicsView()->setDragMode(QGraphicsView::NoDrag);
    }
  }

}
