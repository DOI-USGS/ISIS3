/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/09/09 17:07:41 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
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
  GaussianStretch::GaussianStretch (Histogram &histogram, const double mean, const double standardDeviation) {
    GaussianDistribution dis(mean, standardDeviation);

    p_stretch.ClearPairs();
    p_stretch.AddPair(histogram.Minimum(), histogram.Minimum());
    double lastvalue = histogram.Minimum();
    for (int i=1; i<=histogram.Bins()-1; i++) {
      double percent = 100.0 * (double)i / (double)histogram.Bins();
      double input = histogram.Percent(percent);
      // stretch pairs must be monotonically increasing
      if (lastvalue + DBL_EPSILON > input) continue;
      if (fabs(input - lastvalue) < 100.0 * DBL_EPSILON) continue;
      double output = dis.InverseCumulativeDistribution(percent);
      p_stretch.AddPair(input, output);
      lastvalue = input;
    }

    if (histogram.Maximum() > lastvalue) {
      if (abs(histogram.Maximum() - lastvalue) > 100 * DBL_EPSILON) {
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
  double GaussianStretch::Map (const double value) const {
    return p_stretch.Map(value);
  }
}
