/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "cnetpvl2bin.h"

using namespace std;
using namespace Isis;

namespace Isis{
  /**
    * Default method for cnetpvl2bin that takes a UI object from the
    * application, parses the necessary UI elements, and writes the control
    * network to a binary file.
    *
    * @param ui UserInterface file generated using the cnetpvl2bin file.
    *
    * @param progress A Progress object through which the UI can access progress
    *                 reports.
    */
  void cnetpvl2bin(UserInterface &ui, Progress *progress) {
    ControlNet cnet;
    cnet.ReadControl(ui.GetFileName("FROM"), progress);
    cnetpvl2bin(cnet, ui, progress);
  }


  /**
    * Give a control network and criteria passed in through the UI, write the
    * control network to a binary file.
    *
    * @param cnet A control network object.
    *
    * @param ui UserInterface file generated using the cnetpvl2bin file.
    *
    * @param progress A Progress object through which the UI can access progress
    *                 reports.
    */
  void cnetpvl2bin(ControlNet &cnet, UserInterface &ui, Progress *progress) {

    progress->SetText("Writing Control Network...");
    progress->SetMaximumSteps(1);
    progress->CheckStatus();
    cnet.Write(ui.GetFileName("TO"));
    progress->CheckStatus();
  }
}
