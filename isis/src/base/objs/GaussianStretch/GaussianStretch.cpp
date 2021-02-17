/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "GaussianStretch.h"
#include "GaussianDistribution.h"
#include "Stretch.h"
#include "Message.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;
namespace Isis {
  /**
   * Constructs a gaussian stretch object.
   *
   * @param histogram The input histogram
   * @param mean The mean of the output distribution
   * @param standardDeviation The standard deviation of the output
   *               distribution
   */
  GaussianStretch::GaussianStretch(Histogram &histogram, const double mean, const double standardDeviation) {
    GaussianDistribution dis(mean, standardDeviation);

    p_stretch.ClearPairs();
    p_stretch.AddPair(histogram.Minimum(), histogram.Minimum());
    double lastvalue = histogram.Minimum();
    for(int i = 1; i <= histogram.Bins() - 1; i++) {
      double percent = 100.0 * (double)i / (double)histogram.Bins();
      double input = histogram.Percent(percent);
      // stretch pairs must be monotonically increasing
      if(lastvalue + DBL_EPSILON > input) continue;
      if(fabs(input - lastvalue) < 100.0 * DBL_EPSILON) continue;
      double output = dis.InverseCumulativeDistribution(percent);
      p_stretch.AddPair(input, output);
      lastvalue = input;
    }

    if(histogram.Maximum() > lastvalue) {
      if(abs(histogram.Maximum() - lastvalue) > 100 * DBL_EPSILON) {
        p_stretch.AddPair(histogram.Maximum(), histogram.Maximum());
      }
    }
  }

  /**
  * Maps an input value to an output value based on the gaussian
  * distribution.
  *
  * @param value Value to map
  *
  * @return double The mapped output value is returned by this method
  */
  double GaussianStretch::Map(const double value) const {
    return p_stretch.Map(value);
  }
}
