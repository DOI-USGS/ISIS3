#ifndef ViewportMdiSubWindow_h
#define ViewportMdiSubWindow_h

#include <QMdiSubWindow>

#include <QPointer>

namespace Isis {
  class Cube;
  class CubeViewport;
  class MdiCubeViewport;

  /**
   * This is an actual viewport window in qview/qnet/etc.
   *
   * @author 2012-05-29 Steven Lambright
   *
   * @internal
   *   @history 2012-05-29 Steven Lambright - Original implementation
   *   @history 2012-07-27 Tracie Sucharski - Added closeViewport signal and connect to the
   *                          CubeViewport's viewportClosed signal.  This was done so that tools
   *                          can respond to the user closing a viewport rather than when the
   *                          application exits.
   */
  class ViewportMdiSubWindow : public QMdiSubWindow {
      Q_OBJECT

    public:
      
      /**
       * Constrctor for the ViewportMdiSubWindow
       * 
       * @param cubeToView The cube to open
       * @param parent The parent container
       */ 
      ViewportMdiSubWindow(Cube *cubeToView, QWidget *parent = NULL);
      
      //! Deconstructor for ViewportMdiSubWindow
      ~ViewportMdiSubWindow();

      /**
       * Grabs the viewport
       * 
       * @return the viewport to return
       * 
       */
      MdiCubeViewport *viewport();

    signals:
      /**
       * This method closes the viewport
       * 
       * @param vp The viewport to close.
       */
      void closeViewport(CubeViewport *vp);

    protected:
      /**
       * This method is called as the closeEvent
       * 
       * @param e The closeEvent
       * 
       */
      virtual void closeEvent(QCloseEvent *e);

    private:
      /**
       * Disables copy 
       * 
       */
      Q_DISABLE_COPY(ViewportMdiSubWindow);

    private:
      //! Pointer to the MdiCubeViewports
      QPointer<MdiCubeViewport> m_viewport;
  };
};

#endif
