/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include "Preference.h"
#include "VariableLineScanCameraDetectorMap.h"

/**
 *
 * Unit test for VariableLineScanCameraDetectorMap.
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2016-10-27 Kristin Berry - Added tests for exposureDuration and lineRate methods.
 *                           References #4476.
 */   
using namespace std;
int main() {
  Isis::Preference::Preferences(true);
  cout << "This class will be tested by the applications and the individual Camera models." << endl;

  Isis::Camera *parent = 0;
  vector< Isis::LineRateChange > lineRates;
  lineRates.push_back(Isis::LineRateChange(1,0,2.7));
  lineRates.push_back(Isis::LineRateChange(20,54,0.3));
  lineRates.push_back(Isis::LineRateChange(100,78,5.2));

  Isis::VariableLineScanCameraDetectorMap detectorMap(parent, lineRates);

  cout << "Testing line rate change accessor..." << endl;
  Isis::LineRateChange startRateChange = detectorMap.lineRate(1);
  cout << "    First line rate change..." << endl;
  cout << "        Start Line: " << startRateChange.GetStartLine() << endl;
  cout << "        Start ET: " << startRateChange.GetStartEt() << endl;
  cout << "        Scan Rate: " << startRateChange.GetLineScanRate() << endl << endl;

  Isis::LineRateChange secondRateChange = detectorMap.lineRate(20);
  cout << "    Second line rate change..." << endl;
  cout << "        Start Line: " << secondRateChange.GetStartLine() << endl;
  cout << "        Start ET: " << secondRateChange.GetStartEt() << endl;
  cout << "        Scan Rate: " << secondRateChange.GetLineScanRate() << endl << endl;

  cout << "Testing exposure duration accessor:..." << endl;
  cout << "Exposure duration at line 110: " << detectorMap.exposureDuration(1,110,1) << endl;
}
