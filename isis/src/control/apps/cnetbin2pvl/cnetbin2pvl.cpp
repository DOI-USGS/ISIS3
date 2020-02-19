#include "cnetbin2pvl.h"

using namespace std;
using namespace Isis;

namespace Isis{
  void cnetbin2pvl(UserInterface &ui) {
    // Build a histogram from the control net
    Progress progress;
    ControlNet cnet;
    cnet.ReadControl(ui.GetFileName("FROM"), &progress);

    cnetbin2pvl(cnet, progress, ui);
  }

  void cnetbin2pvl(ControlNet &cnet, Progress &progress, UserInterface &ui) {
    progress.SetText("Writing Control Network...");
    progress.SetMaximumSteps(1);
    progress.CheckStatus();
    cnet.Write(ui.GetFileName("TO"), true);
    progress.CheckStatus();
  }
}
