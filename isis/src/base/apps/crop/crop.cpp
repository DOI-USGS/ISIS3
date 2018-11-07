#include "Isis.h"

#include "Apps.h"

using namespace Isis;

// Globals and prototypes
int ss, sl, sb;
int ns, nl, nb;
int sinc, linc;
Cube *cube = NULL;
LineManager *in = NULL;

void IsisMain() {
  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  std::vector<char *> args = ui.getArgs();
  crop(args);

  // not super worried about this for now
  // Write the results to the log
  // Application::Log(results);
}
