#ifndef ProjectItemViewMenu_h
#define ProjectItemViewMenu_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QMenu>

namespace Isis {

  /**
   * QMenu subclass that overrides the closeEvent. Used in views to disable
   * actions when a menu is visible and a user clicks outside of a view.
   *
   * @author 2018-06-27 Kaitlyn Lee
   *
   * @internal
   *   @history 2018-06-27 Kaitlyn Lee - Original version.
   */

  class ProjectItemViewMenu : public QMenu {
    Q_OBJECT

    public:
      ProjectItemViewMenu(const QString &title, QWidget *parent = 0) : QMenu(title, parent){};

    signals:
      void menuClosed();

    private:
      void closeEvent(QCloseEvent *event);
  };
}

#endif
