#ifndef ViewportMdiSubWindow_h
#define ViewportMdiSubWindow_h

#include <QMdiSubWindow>

#include <QPointer>

namespace Isis {
  class Cube;
  class MdiCubeViewport;

  /**
   * This is an actual viewport window in qview/qnet/etc.
   *
   * @author 2012-05-29 Steven Lambright
   *
   * @internal
   *   @history 2012-05-29 Steven Lambright - Original implementation
   */
  class ViewportMdiSubWindow : public QMdiSubWindow {
      Q_OBJECT

    public:
      ViewportMdiSubWindow(Cube *cubeToView, QWidget *parent = NULL);
      ~ViewportMdiSubWindow();

      MdiCubeViewport *viewport();

    protected:
      virtual void closeEvent(QCloseEvent *e);

    private:
      Q_DISABLE_COPY(ViewportMdiSubWindow);

    private:
      QPointer<MdiCubeViewport> m_viewport;
  };
};

#endif
