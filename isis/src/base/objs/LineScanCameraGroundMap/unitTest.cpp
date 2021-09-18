#include "Preference.h"
#include <iostream>
#include "Preference.h"
#include "IException.h"
#include "CameraPointInfo.h"


using namespace std;
using namespace Isis;
int main() {
  Preference::Preferences(true);
  NaifContextReference naif_reference;
  auto naif = NaifContext::acquire();
  
  cerr << "This class is mostly tested by the applications and the individual Camera models." << endl;

  //create a camera for the test cube
  QString inputFile = "$base/testData/LRONAC_M139722912RE_cropped.cub";
  CameraPointInfo campt;  
  campt.SetCube(inputFile);

  cerr << "attempting to back project a point behind the planet into the image (this should throw an error)\n";
  try {
    campt.SetGround(naif, 90.0, 0.0, true);
    
  }catch (IException &e) {
    e.print();
  }
}
