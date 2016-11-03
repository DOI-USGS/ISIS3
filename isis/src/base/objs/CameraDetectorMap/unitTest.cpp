#include <iostream>

#include "CameraDetectorMap.h"
#include "IException.h"
#include "Preference.h"

using namespace std;

/**
 *
 * Unit test for CameraDetectorMap.
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2016-10-27 Kristin Berry - Added test for exposureDuration method. References #4476.
 */   
int main() {
  Isis::Preference::Preferences(true);
  cout << "This class will be tested by the applications and the individual Camera models." << endl;

  Isis::CameraDetectorMap detectorMap;
  
  cout << "Testing framing camera exposureDuration error throw." << endl;
  try {
    detectorMap.exposureDuration(1, 1, 1);
  }
  catch(Isis::IException &e) {
    e.print();
  }
}
