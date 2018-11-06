#include "Isis.h"

#include <cmath>

#include "Apps.h"
#include "Cube.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "FileName.h"
#include "IException.h"
#include "Projection.h"
#include "AlphaCube.h"
#include "Table.h"
#include "SubArea.h"

using namespace std;
using namespace Isis;

// Globals and prototypes
int ss, sl, sb;
int ns, nl, nb;
int sinc, linc;
Cube *cube = NULL;
LineManager *in = NULL;

void crop(Buffer &out);

void IsisMain() {
  ProcessByLine p;

  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();

  Isis::crop(ui.GetAsString("FROM"),
             ui.GetAsString("TO"),
             ui.GetInteger("LINE"),
             ui.GetInteger("SAMPLE"),
             ui.GetInteger("SINC"),
             ui.GetInteger("LINC"),
             ui.GetInteger("NSAMPLES"),
             ui.GetInteger("NLINES"),
             ui.GetBoolean("PROPSPICE"));

  // Write the results to the log
  // Application::Log(results);
}
