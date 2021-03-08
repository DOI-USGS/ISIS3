/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ProjectItemViewMenu.h"

namespace Isis {


  /**
   * Overrides the closeEvent to emit the signal menuClosed().
   * menuClosed() is connected to the slot disableActions() in a view.
   *
   * @param event The close event
   */
  void ProjectItemViewMenu::closeEvent(QCloseEvent *event) {
    emit menuClosed();
  }
}
