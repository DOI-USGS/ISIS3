/**
 * @file
 * $Date: 2006/10/31 23:18:12 $
 * $Revision: 1.1.1.1 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

#include <float.h>
#include <string>
#include "Statistics.h"
#include "iException.h"

using namespace std;
namespace Isis {
  //! Constructs an IsisStats object with accumulators and counters set to zero.
  Statistics::Statistics () {
    Reset ();
  }

  //! Reset all accumulators and counters to zero.
  void Statistics::Reset() {
    p_sum = 0.0;
    p_sumsum = 0.0;
    p_minimum = DBL_MAX;
    p_maximum = -DBL_MAX;
    p_totalPixels = 0.0;
    p_validPixels = 0.0;
    p_nullPixels = 0.0;
    p_lisPixels = 0.0;
    p_lrsPixels = 0.0;
    p_hrsPixels = 0.0;
    p_hisPixels = 0.0;
    p_removedData = false;
  }

  //! Destroys the IsisStats object.
  Statistics::~Statistics () {};

  /**
   * Add an array of doubles to the accumulators and counters.
   * This method can be invoked multiple times (for example: once
   * for each line in a cube) before obtaining statistics.
   *
   * @param[in] data  (const double*)      data to be added to the data
   *                                       set used for statistical calculations
   *
   * @param[in] count (const unsigned int) number of elements in the incoming
   *                                       data to be added
   */
  void Statistics::AddData (const double *data, const unsigned int count) {
    for (unsigned int i=0; i<count; i++) {
      p_totalPixels++;

      if (Isis::IsValidPixel(data[i])) {
        p_sum += data[i];
        p_sumsum += data[i] * data[i];
        if (data[i] < p_minimum) p_minimum = data[i];
        if (data[i] > p_maximum) p_maximum = data[i];
        p_validPixels++;
      } else if (Isis::IsNullPixel(data[i])) {
        p_nullPixels++;
      } else if (Isis::IsHisPixel(data[i])) {
        p_hisPixels++;
      } else if (Isis::IsHrsPixel(data[i])) {
        p_hrsPixels++;
      } else if (Isis::IsLisPixel(data[i])) {
        p_lisPixels++;
      } else {
        p_lrsPixels++;
      }
    }
  }

  /**
   * Remove an array of doubles from the accumulators and counters.
   * Note that is invalidates the absolute minimum and maximum. They
   * will no longer be usable.
   *
   * @param[in]  data  (const double*)      data to be removed from data
   *                                       set used for statistical calculations
   *
   * @param[in]  count (const unsigned int) number of elements in the data to be removed
   *
   * @throws Isis::iException::Message
   */
  void Statistics::RemoveData (const double *data, const unsigned int count) {
    p_removedData = true;

    for (unsigned int i=0; i<count; i++) {
      p_totalPixels--;

      if (Isis::IsValidPixel(data[i])) {
        p_sum -= data[i];
        p_sumsum -= data[i] * data[i];
        p_validPixels--;
      } else if (Isis::IsNullPixel(data[i])) {
        p_nullPixels--;
      } else if (Isis::IsHisPixel(data[i])) {
        p_hisPixels--;
      } else if (Isis::IsHrsPixel(data[i])) {
        p_hrsPixels--;
      } else if (Isis::IsLisPixel(data[i])) {
        p_lisPixels--;
      } else {
        p_lrsPixels--;
      }
    }

    if (p_totalPixels < 0) {
      string m="You are removing non-existant data in [Statistics::RemoveData]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  }

  /**
   * Computes and returns the average.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return (double) average
   */
  double Statistics::Average () const {
    if (p_validPixels < 1.0) return Isis::NULL8;
    return p_sum / p_validPixels;
  }

  /**
   * Computes and returns the standard deviation.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return (double) standard deviation
   */
  double Statistics::StandardDeviation () const {
    if (p_validPixels <= 1.0) return Isis::NULL8;
    return sqrt(Variance());
  }

  /**
   * Computes and returns the variance.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return (double) variance
   *
   * @internal
   * @history 2003-08-27 Jeff Anderson - Modified Variance method to compute
   *                                     using n*(n-1) instead of n*n.
   */
  double Statistics::Variance () const {
    if (p_validPixels <= 1.0) return Isis::NULL8;
    double temp = p_validPixels * p_sumsum - p_sum * p_sum;
    if (temp < 0.0) temp = 0.0; // This should happen unless roundoff occurs
    return temp / ((p_validPixels - 1.0) * p_validPixels);
  }

  /**
   * Returns the absolute minimum double found in all data passed through the
   * AddData method. If there are no valid pixels, then NULL8 is returned.
   *
   * @return (double) current minimum value in data set
   *
   * @throws Isis::iException::Message
   */
  double Statistics::Minimum () const {
    if (p_removedData) {
      string m = "Minimum is invalid since you removed data";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    if (p_validPixels < 1.0) return Isis::NULL8;
    return p_minimum;
  }

  /**
   * Returns the absolute maximum double found in all
   * data passed through the AddData method. If there
   * are no valid pixels, then NULL8 is returned.
   *
   * @return (double) current maximum value in data set
   *
   * @throws Isis::iException::Message
   */
  double Statistics::Maximum () const {
    if (p_removedData) {
      string m = "Maximum is invalid since you removed data";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    if (p_validPixels < 1.0) return Isis::NULL8;
    return p_maximum;
  }

  /**
   * Returns the total number of pixels processed
   * (valid and invalid).
   *
   * @return (double) number of pixels (data) processed
   */
  double Statistics::TotalPixels () const {
    return p_totalPixels;
  }

  /**
   * Returns the total number of valid pixels processed.
   * Only valid pixels are utilized when computing the
   * average, standard deviation, variance, minimum and
   * maximum.
   *
   * @return (double) number of valid pixels (data) processed
   */
  double Statistics::ValidPixels () const {
    return p_validPixels;
  }

  /**
   * Returns the total number of NULL pixels encountered.
   *
   * @return (double) number of NULL pixels (data) processed
   */
  double Statistics::NullPixels () const {
    return p_nullPixels;
  }

  /**
   * Returns the total number of low instrument
   * saturation (LIS) pixels encountered.
   *
   * @return (double) number of LIS pixels (data) processed
   */
  double Statistics::LisPixels () const {
    return p_lisPixels;
  }

  /**
   * Returns the total number of low representation
   * saturation (LRS) pixels encountered.
   *
   * @return (double) number of LRS pixels (data) processed
   */
  double Statistics::LrsPixels () const {
    return p_lrsPixels;
  }

  /**
   * Returns the total number of high instrument
   * saturation (HIS) pixels encountered.
   *
   * @return (double) number of HIS pixels (data) processed
   */
  double Statistics::HisPixels () const {
    return p_hisPixels;
  }

  /**
   * Returns the total number of high representation
   * saturation (HRS) pixels encountered.
   *
   * @return (double) number of HRS pixels (data) processed
   */
  double Statistics::HrsPixels () const {
    return p_hrsPixels;
  }

  /**
   * This method returns a minimum such that X percent
   * of the data will fall with K standard deviations
   * of the average (Chebyshev's Theorem). It can be
   * used to obtain a minimum that does not include
   * statistical outliers.
   *
   * @param[in]  percent (double) The probability that the minimum
   *                is within K standard deviations of the mean.
   *                Default value = 99.5.
   *
   * @return minimum value excluding statistical outliers
   *
   * @throws Isis::iException::Message
   */
  double Statistics::ChebyshevMinimum (const double percent) const {
    if ((percent <= 0.0) || (percent >= 100.0)) {
      string m = "Invalid value for percent";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    if (p_validPixels < 1.0) return Isis::NULL8;
    double k = sqrt (1.0 / (1.0 - percent / 100.0));
    return Average() - k * StandardDeviation();
  }

  /**
   * This method returns a maximum such that
   * X percent of the data will fall with K
   * standard deviations of the average (Chebyshev's
   * Theorem). It can be used to obtain a minimum that
   * does not include statistical outliers.
   *
   * @param[in]  percent (double) The probability that the maximum
   *                is within K standard deviations of the mean.
   *                Default value = 99.5.
   *
   * @return maximum value excluding statistical outliers
   *
   * @throws Isis::iException::Message
   */
  double Statistics::ChebyshevMaximum (const double percent) const {
    if ((percent <= 0.0) || (percent >= 100.0)) {
      string m = "Invalid value for percent";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    if (p_validPixels < 1.0) return Isis::NULL8;
    double k = sqrt (1.0 / (1.0 - percent / 100.0));
    return Average() + k * StandardDeviation();
  }

  /**
   * This method returns the better of the absolute
   * minimum or the Chebyshev minimum. The better
   * value is considered the value closest to the mean.
   *
   * @param[in]  percent (double) The probability that the minimum is within K
   *                standard deviations of the mean (Used to compute
   *                the Chebyshev minimum). Default value = 99.5.
   *
   * @return (double) Best of absolute and Chebyshev minimums
   *
   * @see Statistics::Minimum
   *      Statistics::ChebyshevMinimum
   */
  double Statistics::BestMinimum (const double percent) const {
    if (p_validPixels < 1.0) return Isis::NULL8;
    double min = ChebyshevMinimum (percent);
    if (Minimum() > min) min = Minimum();
    return min;
  }

  /**
   *
   * This method returns the better of the absolute
   * maximum or the Chebyshev maximum. The better value
   * is considered the value closest to the mean.
   *
   * @param[in]  percent (double) The probability that the maximum is within K
   *                standard deviations of the mean (Used to compute
   *                the Chebyshev maximum). Default value = 99.5.
   *
   * @return (double) Best of absolute and Chebyshev maximums
   *
   * @see Statistics::Maximum
   *      Statistics::ChebyshevMaximum
   */
  double Statistics::BestMaximum (const double percent) const {
    if (p_validPixels < 1.0) return Isis::NULL8;
    double max = ChebyshevMaximum (percent);
    if (Maximum() < max) max = Maximum();
    return max;
  }

} // end namespace isis
