#include "Isis.h"
#include "Enlarge.h"
#include "Interpolator.h"
#include "ProcessRubberSheet.h"
#include "Pvl.h"

#include <cmath>

using namespace std;

void IsisMain() {

  Isis::Preference::Preferences(true);

  Isis::ProcessRubberSheet p;
  Isis::Cube *inCube = p.SetInputCube("FROM");
  
  double sampleScale=2.0, lineScale=2.0;
  Isis::Transform *trans = new Isis::Enlarge(inCube, sampleScale, lineScale);

  Isis::Interpolator *interp;
  interp = new Isis::Interpolator(Isis::Interpolator::NearestNeighborType);

  // Calculate the output size. If there is a fractional pixel, round up
  int ons = (int)ceil(inCube->Samples() * sampleScale);
  int onl = (int)ceil(inCube->Lines() * lineScale);
  Isis::Cube *outCube = p.SetOutputCube("TO", ons, onl, inCube->Bands());
  
  cout << "Testing Isis::Enlarge Class ... " << endl;
  p.StartProcess(*trans, *interp);
  trans->UpdateOutputLabel(outCube);
  Isis::Pvl *outLabel = outCube->Label();
  cout << *outLabel;
  p.EndProcess();

  delete trans;
  delete interp;
  remove("/tmp/enlarged.cub");
}


