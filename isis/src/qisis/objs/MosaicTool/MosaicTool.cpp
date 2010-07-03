#include "MosaicTool.h"

#include "ToolPad.h"
#include "Filename.h"

namespace Qisis {
  QStackedWidget *MosaicTool::p_activeToolBarStack = NULL;
  /** 
   * Tool constructor
   * 
   * @param parent
   */
  MosaicTool::MosaicTool (QWidget *parent) : QObject(parent) {      
    p_toolPadAction = NULL;
    p_toolBarWidget = NULL;
    p_active = false;
    std::string tempFilename = Isis::Filename("$base/icons").Expanded();
    p_toolIconDir = tempFilename.c_str();
    
  }


  /**
   * Adds the tool to the qmos application
   * 
   * 
   * @param mmw 
   */
  void MosaicTool::addTo (Qisis::MosaicMainWindow *mmw) {
    addToPermanent(mmw->permanentToolBar());
    addToActive(mmw->activeToolBar());
    addTo(mmw->toolPad());
    //if (!menuName().isEmpty()) {
      //QMenu *menu = mmw->getMenu(menuName());
      //addTo(menu);
    //}
  }


  /** 
   * Adds the MosaicTool to the toolpad
   * 
   * @param toolpad
   */
  void MosaicTool::addTo (Qisis::ToolPad *toolpad) {
    p_toolPadAction = toolPadAction(toolpad);
    if (p_toolPadAction != NULL) {
      toolpad->addAction(p_toolPadAction);
      connect (p_toolPadAction,SIGNAL(toggled(bool)),this,SLOT(activate(bool)));
    }
  }

  
  /** 
   * 
   * 
   * @param toolbar
   */
  void MosaicTool::addToActive (QToolBar *toolbar) {
    if (p_activeToolBarStack == NULL) {
      p_activeToolBarStack = new QStackedWidget(toolbar);
      toolbar->addWidget(p_activeToolBarStack);
    }

    p_toolBarWidget = createToolBarWidget(p_activeToolBarStack);
    if (p_toolBarWidget != NULL) {
      p_activeToolBarStack->addWidget(p_toolBarWidget);
    }
    disableToolBar();
  }


  /** 
   * Activates the tool.
   * 
   * @param on
   */
  void MosaicTool::activate (bool on) {
    if (p_active) {
      if (on) return;
      disableToolBar();
      if (p_toolPadAction != NULL) p_toolPadAction->setChecked(false);
      p_active = false;
    }
    else {
      if (!on) return;
      if (p_toolPadAction != NULL) p_toolPadAction->setChecked(true);
      enableToolBar();
      p_active = true;
    }
    //-----------------------------------------------------
    // Let the tools know when they have been activated or
    // de-activated.
    //-----------------------------------------------------
    emit activated(p_active);
  }


  /** 
   * Disables entire tool bar.
   * 
   */
  void MosaicTool::disableToolBar() {
    if (p_toolBarWidget == NULL) return;
//    if (p_toolBarWidget->isVisible()) p_toolBarWidget->hide();
    p_toolBarWidget->setEnabled(false);
  }


  /** 
   * Enables entire tool bar.
   * 
   */
  void MosaicTool::enableToolBar() {
    if(p_toolBarWidget == NULL) return;
    p_toolBarWidget->setEnabled(true);
    p_activeToolBarStack->setCurrentWidget(p_toolBarWidget);
    updateTool();
  }


  /**
   * This allows the programmer to access which widget created 
   * a tool, since the parent of this tool is a tool bar. 
   * 
   * 
   * @param widget 
   */
  void MosaicTool::setWidget(MosaicWidget *widget){
    p_widget = widget;
  }

}
