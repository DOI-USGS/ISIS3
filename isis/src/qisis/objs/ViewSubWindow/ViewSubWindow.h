#ifndef ViewSubWindow_h
#define ViewSubWindow_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QMainWindow>

namespace Isis {

  /**
   * @brief This class exists to contain detached views from ipce.
   *          the purpose is for it to emit the closeWindow() signal
   *          so that we can track when detached windows are closed.
   *
   * @ingroup Visualization Tools
   *
   * @author 2017-10-25 Adam Goins
   *
   * @internal
   *
   *  @history 2017-10-25 Adam Goins - This class created.
   */
  class ViewSubWindow : public QMainWindow {
      Q_OBJECT

    signals:
      void closeWindow(); //!< Signal called when the window receives a close event

    public:
      ViewSubWindow(QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags());
      virtual ~ViewSubWindow();

    protected:
      virtual void closeEvent(QCloseEvent *event);
  };
};

#endif
