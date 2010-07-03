#include "ScatterPlotTool.h"


#include "MdiCubeViewport.h"
#include "ScatterPlotWindow.h"
#include "ToolPad.h"

namespace Qisis {

  /**
   * ScatterPlotTool constructor.
   * 
   * @param parent 
   */
  ScatterPlotTool::ScatterPlotTool (QWidget *parent): Qisis::Tool(parent){
    p_parent = parent;        
    createWindow();    
  }

  
  /**
   * Instantiates the scatter plot window.
   * 
   */
  void ScatterPlotTool::createWindow(){

    p_mainWindow = new ScatterPlotWindow("Scatter Plot Window", this, p_parent);

  }


  /**
   * Gives the programmer more flexibility on when the action 
   * button for this tool is checked or not. 
   * 
   * 
   * @param checked 
   */
  void ScatterPlotTool::setActionChecked(bool checked){
      p_action->setChecked(checked);
  }

  
  /** 
   * Configure the QAction for this tool.
   * 
   * @param toolpad
   * 
   * @return QAction*
   */
  QAction *ScatterPlotTool::toolPadAction(ToolPad *toolpad) {
    p_action = new QAction(toolpad);
    p_action->setIcon(QPixmap(toolIconDir()+"/scatterplot.png"));
    p_action->setToolTip("Scatter Plot");
    //action->setShortcut(Qt::Key_C);
    QObject::connect(p_action,SIGNAL(activated()),p_mainWindow,SLOT(showConfig()));

    QString text  =
      "<b>Function:</b>  Compare two bands of same image or of a different image. \
      <p><b>Shortcut:</b>nonexsistant right now</p> ";
    p_action->setWhatsThis(text);
    return p_action;
  }


  /**
   * Returns the list of cube viewports currently open in Qview.
   * 
   * 
   * @return CubeViewportList* 
   */
  ScatterPlotTool::CubeViewportList * ScatterPlotTool::getCubeViewportList()
      const
  {
    return cubeViewportList();
  }

}

