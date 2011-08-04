#ifndef WindowTool_h
#define WindowTool_h

#include "Tool.h"

class QAction;

namespace Isis {
  class MdiCubeViewport;

  /**
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *            CubeViewPort.  Fixed include issues
   */
  class WindowTool : public Tool {
      Q_OBJECT

    public:
      WindowTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addTo(Workspace *ws);
      void addToPermanent(QToolBar *toolbar);

    protected:
      //! Returns the menu name.
      QString menuName() const {
        return "&Window";
      };
      void updateTool();
      void addConnections(MdiCubeViewport *cvp);
      void removeConnections(MdiCubeViewport *cvp);

    private slots:
      void linkWindows();
      void unlinkWindows();
      void resizeWindows();
      void changeCursor();

    private:
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

