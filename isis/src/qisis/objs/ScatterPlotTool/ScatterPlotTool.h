#ifndef ScatterPlotTool_h
#define ScatterPlotTool_h

// this is the only include allowed in this file!
#include "Tool.h"


class QAction;
template< class T > class QVector;
class QWidget;

namespace Isis {
  class MdiCubeViewport;
  class ScatterPlotWindow;

  /**
   * @brief Scatter Plot Tool
   *
   * @author Stacy Alley
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *            CubeViewport.  Also fixed some include issues.
   */
  class ScatterPlotTool : public Tool {
      Q_OBJECT

    public:
      ScatterPlotTool(QWidget *parent);
      void setActionChecked(bool checked);

      typedef QVector< MdiCubeViewport * > CubeViewportList;
      CubeViewportList *getCubeViewportList() const;

    protected:
      void createWindow();
      QAction *toolPadAction(ToolPad *pad);


      /**
       * Returns this tool's action
       *
       *
       * @return QAction*
       */
      QAction *toolAction() {
        return p_action;
      };

    private:
      ScatterPlotWindow *p_mainWindow;  //!<The window the scatter plot is displayed in.
      QAction *p_action;  //!< The tool's action
      QWidget *p_parent; //!< The tool's parent.


  };
};




#endif

