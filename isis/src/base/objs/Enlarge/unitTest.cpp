#include "Isis.h"

#include <cmath>

#include <QFile>

#include "Enlarge.h"
#include "Interpolator.h"
#include "ProcessRubberSheet.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  Preference::Preferences(true);

  ProcessRubberSheet p;
  Cube *inCube = p.SetInputCube("FROM");

  double sampleScale=2.0, lineScale=2.0;
  Enlarge *trans = new Enlarge(inCube, sampleScale, lineScale);

  Interpolator *interp = new Interpolator(Interpolator::NearestNeighborType);

  // Calculate the output size. If there is a fractional pixel, round up
  int ons = (int)ceil(inCube->getSampleCount() * sampleScale);
  int onl = (int)ceil(inCube->getLineCount() * lineScale);
  Cube *outCube = p.SetOutputCube("TO", ons, onl, inCube->getBandCount());

  cerr << "Testing Enlarge Class ... " << endl;
  p.StartProcess(*trans, *interp);
  trans->UpdateOutputLabel(outCube);
  Pvl *outLabel = outCube->getLabel();
  outLabel->DeleteObject("History");
  cerr << *outLabel;
  p.EndProcess();

  delete trans;
  delete interp;

  UserInterface &ui = Application::GetUserInterface();
  QFile::remove(ui.GetFileName("TO"));
}


