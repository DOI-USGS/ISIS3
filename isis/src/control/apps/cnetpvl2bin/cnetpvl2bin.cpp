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
