#include <iostream>

#include "Preference.h"
#include "PushFrameCameraDetectorMap.h"

using namespace std;

/**
 *
 * Unit test for PushFrameCameraDetectorMap.
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2016-10-27 Kristin Berry - Added test for exposureDuration method.  References #4476.
 */   
int main() {
  Isis::Preference::Preferences(true);
  cout << "This class will be tested by the applications and the individual Camera models." << endl;

  Isis::Camera *parent = 0;
  Isis::PushFrameCameraDetectorMap detectorMap(parent, 5, 2, 10);
  cout << "Testing unset exposure duration..." << endl;
  cout << "ExposureDuration: " << detectorMap.exposureDuration(1,1,1) << endl;

  detectorMap.SetExposureDuration(0.2);
  cout << endl << "Testing set exposure duration..." << endl;
  cout << "ExposureDuration: " << detectorMap.exposureDuration(1,1,1) << endl;
}
