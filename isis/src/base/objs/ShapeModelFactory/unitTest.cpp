#include "ShapeModelFactory.h"
#include "ShapeModel.h"
#include "Camera.h"
#include "Preference.h"
#include "CameraFactory.h"

using namespace std;
using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  // Test simp shape
  string inputFile = "$ISIS3DATA/mgs/testData/ab102401.lev2.cub";
  Cube cube;
  cube.Open(inputFile);
  Pvl &pvl = *cube.Label();
  ShapeModel *sm = ShapeModelFactory::Create(pvl);
  delete sm;
  cube.Close();

  // Test ellipsoid shape
  inputFile = "$ISIS3DATA/galileo/testData/1213r.cub";
  cube.Open(inputFile);
  pvl = *cube.Label();
  sm = ShapeModelFactory::Create(pvl);
  delete sm;
  cube.Close();
}
