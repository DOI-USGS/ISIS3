#include "cnetpvl2bin.h"

using namespace std;
using namespace Isis;

namespace Isis{
  void cnetpvl2bin(UserInterface &ui, Progress *progress) {
    ControlNet cnet;
    cnet.ReadControl(ui.GetFileName("FROM"), progress);
    cnetpvl2bin(cnet, ui, progress);
  }

  void cnetpvl2bin(ControlNet &cnet, UserInterface &ui, Progress *progress) {
    progress->SetText("Writing Control Network...");
    progress->SetMaximumSteps(1);
    progress->CheckStatus();
    cnet.Write(ui.GetFileName("TO"));
    progress->CheckStatus();
  }
}
