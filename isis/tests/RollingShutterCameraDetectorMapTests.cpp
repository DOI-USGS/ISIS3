#include "RollingShutterCameraDetectorMap.h"
#include "IException.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

TEST(RollingShutterCameraDetectorMapTests, ApplyAndRemoveJitter) {
  std::vector<double> times = {0.000329333333333,
                               0.010428888888889,
                               0.022284888888889};

  std::vector<double> lineCoeffs = {-1.1973143372677,
                                     1.4626764650998,
                                     0.9960730288934};

  std::vector<double> sampleCoeffs = {-3.2335155748071,
                                       1.1186072652055,
                                       2.740121618258};

  Isis::RollingShutterCameraDetectorMap detectorMap(NULL, times, sampleCoeffs, lineCoeffs);

  std::vector<double> lines;
  std::vector<double> samples;

  for (int line = 1; line <= 3; line++) {
    for (int sample = 1; sample <= 3; sample++) {
      std::pair<double, double> removed = detectorMap.removeJitter(sample, line);
      std::pair<double, double> applied = detectorMap.applyJitter(removed.first, removed.second);
      // Test tolerances match iteration tolerance in RollingShutterCameraDetectorMap::applyJitter
      EXPECT_NEAR(sample, applied.first, 1e-7);
      EXPECT_NEAR(line, applied.second, 1e-7);
    }
  }
}

