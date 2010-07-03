#include "MosaicPanTool.h"
#include "MosaicMainWindow.h"

#include <QMenu>

namespace Qisis {
  /**
   * MosaicPanTool constructor
   * 
   * 
   * @param parent 
   */
  MosaicPanTool::MosaicPanTool (QWidget *parent) : Qisis::MosaicTool(parent) {
    p_parent = (MosaicWidget *)parent;
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
  QAction *MosaicPanTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir()+"/move.png"));
    //action->setIcon(QPixmap("/work1/salley/icons/mActionMoveVertex.png"));
    action->setToolTip("Pan (P)");
    action->setShortcut(Qt::Key_P);
    QString text  =
      "<b>Function:</b>  View different areas of the mosaic. \
      <p><b>Shortcut:</b>  P</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Adds the pan action to the given menu.
   * 
   * 
   * @param menu 
   */
  void MosaicPanTool::addToMenu(QMenu *menu) {
    
  }


  /**
   * Creates the widget to add to the tool bar.
   * 
   * 
   * @param parent 
   * 
   * @return QWidget* 
   */
  QWidget *MosaicPanTool::createToolBarWidget (QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);
    return hbox;
  }

 
 /** 
  *
  * 
  */
  void MosaicPanTool::updateTool() {
   
    const QList<MosaicItem *> items =  p_parent->allMosaicItems();
    //const QList<MosaicItem *> &items =  p_parent->mosaicItems();
    if(isActive()) {
      for(int i = 0; i < items.size(); i++) {
        items[i]->setFlag(QGraphicsItem::ItemIsSelectable, false);
      }
      getGraphicsView()->setDragMode(QGraphicsView::ScrollHandDrag);
    } else {
      for(int i = 0; i < items.size(); i++) {
       items[i]->setFlag(QGraphicsItem::ItemIsSelectable, true);
      }
      getGraphicsView()->setDragMode(QGraphicsView::NoDrag);
    }
  }

}
