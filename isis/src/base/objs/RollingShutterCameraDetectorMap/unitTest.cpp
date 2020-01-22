#include "RollingShutterCameraDetectorMap.h"
#include "Preference.h"

#include <iomanip>
#include <iostream>

using namespace Isis;
using namespace std;

/**
 *
 * Unit test for RollingShutterCameraDetectorMap.
 *
 * @author 2018-04-07 Makayla Shepherd and Ian Humphrey
 */
int main() {
  Preference::Preferences(true);

  std::vector<double> times = {0.000329333333333,
                               0.010428888888889,
                               0.022284888888889};
                               
  std::vector<double> lineCoeffs = {-1.1973143372677,
                                    1.4626764650998,
                                    0.9960730288934};
                                      
  std::vector<double> sampleCoeffs = {-3.2335155748071,
                                      1.1186072652055,
                                      2.740121618258};
                                    
  RollingShutterCameraDetectorMap map(NULL, times, sampleCoeffs, lineCoeffs);
  cout << "    Removed, Applied\n";
  std::vector<double> lines;
  std::vector<double> samples;
  for (int line = 1; line <= 3; line++) {
    for (int sample = 1; sample <= 3; sample++) {
      std::pair<double, double> removed = map.removeJitter(sample, line);
      std::pair<double, double> applied = map.applyJitter(removed.first, removed.second);
//        std::pair<double, double> applied(0.0, 0.0);
       cout << "Line " << line << ": " << removed.second << ", " << applied.second << "\t";
       cout << "samp " << sample << ": " << removed.first << ", " << applied.first << endl;
      
      lines.push_back(applied.second);
      samples.push_back(applied.first);
      
    }
  }
}
