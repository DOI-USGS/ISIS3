/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include "LineScanCameraDetectorMap.h"
#include "Preference.h"

using namespace std;

/**
 *
 * Unit test for LineScanCameraDetectorMap.
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2016-10-27 Kristin Berry - Added test for exposureDuration method. References #4476.
 */   
int main() {
  Isis::Preference::Preferences(true);
  cout << "This class will be tested by the applications and the individual Camera models." << endl;

  Isis::Camera *parent = 0;
  Isis::LineScanCameraDetectorMap detectorMap(parent, 0, 2.6);

  cout << "Exposure duration: " << detectorMap.exposureDuration(1,1,1) << endl;
}
