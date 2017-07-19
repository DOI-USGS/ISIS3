#ifndef WindowTool_h
#define WindowTool_h

#include "Tool.h"

class QAction;
class QMdiArea;

namespace Isis {
  class MdiCubeViewport;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *                           CubeViewPort.  Fixed include issues
   *   @history 2011-09-20 Steven Lambright - Cursors should remain consistent
   *                           between all viewports now. Fixes #195.
   *   @history 2017-07-19 Marjorie Hahn and Tracie Sucharski - Implemented new
   *                           viewport tiling scheme.
   */
  class WindowTool : public Tool {
      Q_OBJECT

    public:
      WindowTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addTo(Workspace *ws);
      void addToPermanent(QToolBar *toolbar);

      /** 
       * @return the menu name
       */
      QString menuName() const {
        return "&Window";
      }

    protected:
      void updateTool();
      void addConnections(MdiCubeViewport *cvp);
      void removeConnections(MdiCubeViewport *cvp);

    private slots:
      void changeCursor();
      void linkWindows();
      void unlinkWindows();
      void resizeWindows();
      void updateViewportCursor(MdiCubeViewport *);
      void tileViewports();

    private:
      int viewportSize();

    private:
      QMdiArea *p_mdiArea;         //!< area where viewports are displayed
      QAction *p_cascadeWindows;   //!< cascade windows action
      QAction *p_tileWindows;      //!< tile windows action
      QAction *p_resizeWindows;    //!< resize windows action
      QAction *p_prevWindow;       //!< previous window action
      QAction *p_nextWindow;       //!< next window action
      QAction *p_closeWindow;      //!< close window action
      QAction *p_closeAllWindows;  //!< close all action
      QAction *p_linkWindow;       //!< link window action
      QAction *p_linkAllWindows;   //!< link all windows action
      QAction *p_unlinkAllWindows; //!< unlink all windows action
      QAction *p_changeCursor;     //!< changes the cursor when it moves over the viewport
  };
};

#endif

