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
      ViewportMdiSubWindow(Cube *cubeToView, QWidget *parent = NULL);
      ~ViewportMdiSubWindow();

      MdiCubeViewport *viewport();

    signals:
      void closeViewport(CubeViewport *vp);

    protected:
      virtual void closeEvent(QCloseEvent *e);

    private:
      Q_DISABLE_COPY(ViewportMdiSubWindow);

    private:
      QPointer<MdiCubeViewport> m_viewport;
  };
};

#endif
