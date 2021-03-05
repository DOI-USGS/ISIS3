/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
  QString inputFile = "$ISISTESTDATA/isis/src/base/unitTestData/LRONAC_M139722912RE_cropped.cub";
  CameraPointInfo campt;  
  campt.SetCube(inputFile);

  cerr << "attempting to back project a point behind the planet into the image (this should throw an error)\n";
  try {
    campt.SetGround(90.0, 0.0, true);
    
  }catch (IException &e) {
    e.print();
  }
}
