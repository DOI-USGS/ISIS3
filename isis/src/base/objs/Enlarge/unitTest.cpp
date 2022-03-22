/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
  int ons = (int)ceil(inCube->sampleCount() * sampleScale);
  int onl = (int)ceil(inCube->lineCount() * lineScale);
  Cube *outCube = p.SetOutputCube("TO", ons, onl, inCube->bandCount());

  cerr << "Testing Enlarge Class ... " << endl;
  p.StartProcess(*trans, *interp);
  PvlGroup results = trans->UpdateOutputLabel(outCube);
  cerr << results << endl;
  Pvl *outLabel = outCube->label();
  outLabel->deleteObject("History");
  cerr << *outLabel;
  p.EndProcess();

  delete trans;
  delete interp;

  UserInterface &ui = Application::GetUserInterface();
  QFile::remove(ui.GetCubeName("TO"));
}


