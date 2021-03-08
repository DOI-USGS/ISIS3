/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ViewSubWindow.h"

namespace Isis {

  /**
   * Constructs a ViewSubWindow object
   */
  ViewSubWindow::ViewSubWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {

  }


  /**
   *  Destructor
   */
  ViewSubWindow::~ViewSubWindow() {

  }



  /**
   * This emits a signal on close so that we can handle removing the window from the
   *
   * @param event
   */
  void ViewSubWindow::closeEvent(QCloseEvent *event) {
    emit closeWindow();
    QMainWindow::closeEvent(event);
  }
}
