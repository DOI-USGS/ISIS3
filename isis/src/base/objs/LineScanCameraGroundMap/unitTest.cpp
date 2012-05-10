#include "Preference.h"
#include <iostream>
#include "Preference.h"
#include "IException.h"
#include "CameraPointInfo.h"


using namespace std;
using namespace Isis;
int main() {
  Isis::Preference::Preferences(true);
  cerr << "This class is mostly tested by the applications and the individual Camera models." << endl;

  //create a camera for the test cube
  string inputFile = "$base/testData/LRONAC_M139722912RE_cropped.cub";
  CameraPointInfo campt;  
  campt.SetCube(inputFile);

  cerr << "attempting to back project a point behind the planet into the image (this should throw an error)\n";
  try {
    campt.SetGround(90.0, 0.0, true);
    
  }catch (IException &e) {
    e.print();
  }
}
