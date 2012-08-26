#include "ShapeModelFactory.h"
#include "ShapeModel.h"
#include "Camera.h"
#include "Preference.h"
#include "CameraFactory.h"

using namespace std;
using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  // Test demshape
  string inputFile = "$ISIS3DATA/mgs/testData/ab102401.lev2.cub";
  Cube cube;
  cube.open(inputFile);
  Camera *c = cube.getCamera();
  vector<Distance> radii(3,Distance());
  radii = c->target()->radii();
  Pvl pvl = *cube.getLabel();
  Target targ(pvl);
  targ.setRadii(radii);
  ShapeModel *sm = ShapeModelFactory::Create(&targ, pvl);
  cout << "Successfully created shape " << sm->name() << endl;
  delete sm;
  cube.close();

  // Test ellipsoid shape
  inputFile = "$ISIS3DATA/galileo/testData/1213r.cub";
  cube.open(inputFile);
  c = cube.getCamera();
  radii = c->target()->radii();
  pvl = *cube.getLabel();
  Target targ2(pvl);
  targ2.setRadii(radii);
  sm = ShapeModelFactory::Create(&targ2, pvl);
  cout << "Successfully created shape " << sm->name() << endl;
  delete sm;
  cube.close();

  // Test plane shape  TBD
  // inputFile = "$ISIS3DATA/;
  // cube.open(inputFile);
  // c = cube.getCamera();
  // radii = c->target()->radii();
  // pvl = *cube.getLabel();
  // Target targ2(pvl);
  // targ3.setRadii(radii);
  // sm = ShapeModelFactory::Create(&targ3, pvl);
  // cout << "Successfully created shape " << sm->name() << endl;
  // delete sm;
  // cube.close();
}
