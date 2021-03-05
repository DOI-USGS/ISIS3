/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "cnetbin2pvl.h"

using namespace std;
using namespace Isis;

namespace Isis{
  void cnetbin2pvl(UserInterface &ui, Progress *progress) {
    ControlNet cnet;
    cnet.ReadControl(ui.GetFileName("FROM"), progress);

    cnetbin2pvl(cnet, ui, progress);
  }

  void cnetbin2pvl(ControlNet &cnet, UserInterface &ui, Progress *progress) {
    progress->SetText("Writing Control Network...");
    progress->SetMaximumSteps(1);
    progress->CheckStatus();
    cnet.Write(ui.GetFileName("TO"), true);
    progress->CheckStatus();
  }
}
