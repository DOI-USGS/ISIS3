/**
 * @file
 * $Date: 2010/03/19 20:34:55 $
 * $Revision: 1.6 $
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

#include <QString>

#include <float.h>

#include "Statistics.h"
#include "IException.h"
#include "IString.h"

using namespace std;

namespace Isis {
  //! Constructs an IsisStats object with accumulators and counters set to zero.
  Statistics::Statistics() {
    SetValidRange();
    Reset();
  }




  //! Destroys the IsisStats object.
  Statistics::~Statistics() {
  }



  //! Reset all accumulators and counters to zero.
  void Statistics::Reset() {
    m_sum = 0.0;
    m_sumsum = 0.0;
    m_minimum = DBL_MAX;
    m_maximum = -DBL_MAX;
    m_totalPixels = 0;
    m_validPixels = 00;
    m_nullPixels = 0;
    m_lisPixels = 0;
    m_lrsPixels = 0;
    m_hrsPixels = 0;
    m_hisPixels = 0;
    m_overRangePixels = 0;
    m_underRangePixels = 0;
    m_removedData = false;
  }



  /** 
   * This method sets the Statistics class members with known values instead of 
   * using the AddData() and RemoveData() methods to do so. 
   */ 
  void Statistics::set(int validPixels, int nullPixels, int lrsPixels, int lisPixels,
           int hrsPixels, int hisPixels, int underRangePixels, int overRangePixels,
           double sum,  double minimum,  double maximum, double validMinimum,
           double validMaximum, bool removedData) {
    m_sum              = sum;
    m_sumsum           = sum*sum;
    m_minimum          = minimum;
    m_maximum          = maximum;
    m_totalPixels      = validPixels + nullPixels + lrsPixels + lisPixels
                         + hrsPixels + hisPixels + underRangePixels + overRangePixels;
    m_validPixels      = validPixels;
    m_nullPixels       = nullPixels;
    m_lrsPixels        = lrsPixels;
    m_lisPixels        = lisPixels;
    m_hrsPixels        = hrsPixels;
    m_hisPixels        = hisPixels;
    m_underRangePixels = underRangePixels;
    m_overRangePixels  = overRangePixels;
    m_removedData      = removedData;
  }



  /**
   * Add an array of doubles to the accumulators and counters.
   * This method can be invoked multiple times (for example: once
   * for each line in a cube) before obtaining statistics.
   *
   * @param data The data to be added to the data set used for statistical
   *    calculations.
   *
   * @param count The number of elements in the incoming data to be added.
   */
  void Statistics::AddData(const double *data, const unsigned int count) {
    for(unsigned int i = 0; i < count; i++) {
      double value = data[i];
      //Calls the inline AddData method  --see .h file.
      AddData(value);
    }
  }



  /**
   * Add a double to the accumulators and counters. This method
   * can be invoked multiple times (for example: once for each
   * pixel in a cube) before obtaining statistics.
   *
   * @param data The data to be added to the data set used for statistical
   *    calculations.
   *
   */
  void Statistics::AddData(const double data) {
    m_totalPixels++;
    if(Isis::IsValidPixel(data) && InRange(data)) {
      m_sum += data;
      m_sumsum += data * data;
      if(data < m_minimum) m_minimum = data;
      if(data > m_maximum) m_maximum = data;
      m_validPixels++;
    }
    else if(Isis::IsNullPixel(data)) {
      m_nullPixels++;
    }
    else if(Isis::IsHisPixel(data)) {
      m_hisPixels++;
    }
    else if(Isis::IsHrsPixel(data)) {
      m_hrsPixels++;
    }
    else if(Isis::IsLisPixel(data)) {
      m_lisPixels++;
    }
    else if(Isis::IsLrsPixel(data)) {
      m_lrsPixels++;
    }
    else if(AboveRange(data)) {
      m_overRangePixels++;
    }
    else {
      m_underRangePixels++;
    }
  }



  /**
   * Remove an array of doubles from the accumulators and counters.
   * Note that is invalidates the absolute minimum and maximum. They
   * will no longer be usable.
   *
   * @param data The data to be removed from data set used for statistical
   *    calculations.
   *
   * @param count The number of elements in the data to be removed.
   *
   * @throws IException::Message RemoveData is trying to remove data that
   *    doesn't exist.
   */
  void Statistics::RemoveData(const double *data, const unsigned int count) {
    m_removedData = true;

    for(unsigned int i = 0; i < count; i++) {
      m_totalPixels--;

      if(IsValidPixel(data[i]) && InRange(data[i])) {
        m_sum -= data[i];
        m_sumsum -= data[i] * data[i];
        m_validPixels--;
      }
      else if(Isis::IsNullPixel(data[i])) {
        m_nullPixels--;
      }
      else if(Isis::IsHisPixel(data[i])) {
        m_hisPixels--;
      }
      else if(Isis::IsHrsPixel(data[i])) {
        m_hrsPixels--;
      }
      else if(Isis::IsLisPixel(data[i])) {
        m_lisPixels--;
      }
      else if(Isis::IsLrsPixel(data[i])) {
        m_lrsPixels--;
      }
      else if(AboveRange(data[i])) {
        m_overRangePixels--;
      }
      else {
        m_underRangePixels--;
      }
    }

    if(m_totalPixels < 0) {
      QString msg = "You are removing non-existant data in [Statistics::RemoveData]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  void Statistics::RemoveData(const double data) {
    RemoveData(&data, 1);
  }

  void Statistics::SetValidRange(const double minimum, const double maximum) {
    m_validMinimum = minimum;
    m_validMaximum = maximum;

    if(m_validMaximum < m_validMinimum) {
      // get the min and max DN values in the chosen range
      QString msg = "Invalid Range: Minimum [" + toString(minimum) 
                    + "] must be less than the Maximum [" + toString(maximum) + "].";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }
  /**
   * Computes and returns the average.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return The Average
   */
  double Statistics::Average() const {
    if(m_validPixels < 1) return Isis::NULL8;
    return m_sum / m_validPixels;
  }

  /**
   * Computes and returns the standard deviation.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return The standard deviation
   */
  double Statistics::StandardDeviation() const {
    if(m_validPixels <= 1) return Isis::NULL8;
    return sqrt(Variance());
  }

  /**
   * Computes and returns the variance.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return The variance
   *
   * @internal
   * @history 2003-08-27 Jeff Anderson - Modified Variance method to compute
   *                                     using n*(n-1) instead of n*n.
   */
  double Statistics::Variance() const {
    if(m_validPixels <= 1) return Isis::NULL8;
    double temp = m_validPixels * m_sumsum - m_sum * m_sum;
    if(temp < 0.0) temp = 0.0;  // This should happen unless roundoff occurs
    return temp / ((m_validPixels - 1.0) * m_validPixels);
  }

  /**
   * Computes and returns the rms.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return The rms (root mean square)
   *
   * @internal
   * @history 2011-06-13 Ken Edmundson.
   */
  double Statistics::Rms() const {
    if(m_validPixels < 1) return Isis::NULL8;
    double temp = m_sumsum / m_validPixels;
    if(temp < 0.0) temp = 0.0;
    return sqrt(temp);
  }

  /**
   * Returns the absolute minimum double found in all data passed through the
   * AddData method. If there are no valid pixels, then NULL8 is returned.
   *
   * @return Current minimum value in data set.
   *
   * @throws Isis::IException::Message The data set is blank, so the minimum is
   *    invalid.
   */
  double Statistics::Minimum() const {
    if(m_removedData) {
      QString msg = "Minimum is invalid since you removed data";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(m_validPixels < 1) return Isis::NULL8;
    return m_minimum;
  }

  /**
   * Returns the absolute maximum double found in all
   * data passed through the AddData method. If there
   * are no valid pixels, then NULL8 is returned.
   *
   * @return Current maximum value in data set
   *
   * @throws Isis::IException::Message The data set is blank, so the maximum is
   *    invalid.
   */
  double Statistics::Maximum() const {
    if(m_removedData) {
      QString msg = "Maximum is invalid since you removed data";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(m_validPixels < 1) return Isis::NULL8;
    return m_maximum;
  }

  /**
   * Returns the total number of pixels processed
   * (valid and invalid).
   *
   * @return The number of pixels (data) processed
   */
  BigInt Statistics::TotalPixels() const {
    return m_totalPixels;
  }

  /**
   * Returns the total number of valid pixels processed.
   * Only valid pixels are utilized when computing the
   * average, standard deviation, variance, minimum and
   * maximum.
   *
   * @return The number of valid pixels (data) processed
   */
  BigInt Statistics::ValidPixels() const {
    return m_validPixels;
  }

  /**
   * Returns the total number of pixels over the valid range
   *   encountered.
   *
   * @return The number of pixels less than the ValidMaximum() processed
   */
  BigInt Statistics::OverRangePixels() const {
    return m_overRangePixels;
  }

  /**
   * Returns the total number of pixels under the valid range
   *   encountered.
   *
   * @return The number of pixels less than the ValidMinimum() processed
   */
  BigInt Statistics::UnderRangePixels() const {
    return m_underRangePixels;
  }

  /**
   * Returns the total number of NULL pixels encountered.
   *
   * @return The number of NULL pixels (data) processed
   */
  BigInt Statistics::NullPixels() const {
    return m_nullPixels;
  }

  /**
   * Returns the total number of low instrument
   * saturation (LIS) pixels encountered.
   *
   * @return The number of LIS pixels (data) processed
   */
  BigInt Statistics::LisPixels() const {
    return m_lisPixels;
  }

  /**
   * Returns the total number of low representation
   * saturation (LRS) pixels encountered.
   *
   * @return The number of LRS pixels (data) processed
   */
  BigInt Statistics::LrsPixels() const {
    return m_lrsPixels;
  }

  /**
   * Returns the total number of high instrument
   * saturation (HIS) pixels encountered.
   *
   * @return The number of HIS pixels (data) processed
   */
  BigInt Statistics::HisPixels() const {
    return m_hisPixels;
  }

  /**
   * Returns the total number of high representation
   * saturation (HRS) pixels encountered.
   *
   * @return The number of HRS pixels (data) processed
   */
  BigInt Statistics::HrsPixels() const {
    return m_hrsPixels;
  }

  /**
   * Returns the total number of pixels outside of
   * the valid range encountered.
   *
   * @return The number of Out of Range pixels (data) processed
   */
  BigInt Statistics::OutOfRangePixels() const {
    return m_overRangePixels + m_underRangePixels;
  }

  bool Statistics::RemovedData() const {
    return m_removedData;
  }
  /**
   * This method returns a minimum such that X percent
   * of the data will fall with K standard deviations
   * of the average (Chebyshev's Theorem). It can be
   * used to obtain a minimum that does not include
   * statistical outliers.
   *
   * @param percent The probability that the minimum
   *                is within K standard deviations of the mean.
   *                Default value = 99.5.
   *
   * @return Minimum value (excluding statistical outliers)
   *
   * @throws Isis::IException::Message
   */
  double Statistics::ChebyshevMinimum(const double percent) const {
    if((percent <= 0.0) || (percent >= 100.0)) {
      QString msg = "Invalid value for percent";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(m_validPixels < 1) return Isis::NULL8;
    double k = sqrt(1.0 / (1.0 - percent / 100.0));
    return Average() - k * StandardDeviation();
  }

  /**
   * This method returns a maximum such that
   * X percent of the data will fall with K
   * standard deviations of the average (Chebyshev's
   * Theorem). It can be used to obtain a minimum that
   * does not include statistical outliers.
   *
   * @param percent The probability that the maximum
   *                is within K standard deviations of the mean.
   *                Default value = 99.5.
   *
   * @return maximum value excluding statistical outliers
   *
   * @throws Isis::IException::Message
   */
  double Statistics::ChebyshevMaximum(const double percent) const {
    if((percent <= 0.0) || (percent >= 100.0)) {
      QString msg = "Invalid value for percent";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(m_validPixels < 1) return Isis::NULL8;
    double k = sqrt(1.0 / (1.0 - percent / 100.0));
    return Average() + k * StandardDeviation();
  }

  /**
   * This method returns the better of the absolute
   * minimum or the Chebyshev minimum. The better
   * value is considered the value closest to the mean.
   *
   * @param percent The probability that the minimum is within K
   *                standard deviations of the mean (Used to compute
   *                the Chebyshev minimum). Default value = 99.5.
   *
   * @return Best of absolute and Chebyshev minimums
   *
   * @see Statistics::Minimum
   *      Statistics::ChebyshevMinimum
   */
  double Statistics::BestMinimum(const double percent) const {
    if(m_validPixels < 1) return Isis::NULL8;
    double min = ChebyshevMinimum(percent);
    if(Minimum() > min) min = Minimum();
    return min;
  }

  /**
   *
   * This method returns the better of the absolute
   * maximum or the Chebyshev maximum. The better value
   * is considered the value closest to the mean.
   *
   * @param percent The probability that the maximum is within K
   *                standard deviations of the mean (Used to compute
   *                the Chebyshev maximum). Default value = 99.5.
   *
   * @return Best of absolute and Chebyshev maximums
   *
   * @see Statistics::Maximum
   *      Statistics::ChebyshevMaximum
   */
  double Statistics::BestMaximum(const double percent) const {
    if(m_validPixels < 1) return Isis::NULL8;
    double max = ChebyshevMaximum(percent);
    if(Maximum() < max) max = Maximum();
    return max;
  }

  /**
   *
   * This method returns the better of the z-score
   * of the given value. The z-score is the number of
   * standard deviations the value lies above or
   * below the average.
   *
   * @param value The value to calculate the z-score of.
   *
   * @return z-score
   *
   */
  double Statistics::ZScore(const double value) const {
    if(StandardDeviation() == 0) {
      if(value == Maximum()) return 0;
      else {
        QString msg = "Undefined Z-score. Standard deviation is zero and the input value[" 
                      + toString(value) + "] is out of range [" + toString(Maximum()) + "].";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    return (value - Average()) / StandardDeviation();
  }



  QDataStream &Statistics::write(QDataStream &stream) const {
    stream << m_sum
           << m_sumsum
           << m_minimum
           << m_maximum
           << m_validMinimum
           << m_validMaximum
           << (qint32)m_totalPixels
           << (qint32)m_validPixels
           << (qint32)m_nullPixels
           << (qint32)m_lrsPixels
           << (qint32)m_lisPixels
           << (qint32)m_hrsPixels
           << (qint32)m_hisPixels
           << (qint32)m_underRangePixels
           << (qint32)m_overRangePixels
           << m_removedData;
    return stream;
  }



  QDataStream &Statistics::read(QDataStream &stream) {

    qint32 totalPixels, validPixels, nullPixels, lrsPixels, lisPixels,
           hrsPixels, hisPixels, underRangePixels, overRangePixels;

    stream >> m_sum
           >> m_sumsum
           >> m_minimum
           >> m_maximum
           >> m_validMinimum
           >> m_validMaximum
           >> totalPixels
           >> validPixels
           >> nullPixels
           >> lrsPixels
           >> lisPixels
           >> hrsPixels
           >> hisPixels
           >> underRangePixels
           >> overRangePixels
           >> m_removedData;

    return stream;
  }



  QDataStream &operator<<(QDataStream &stream, const Statistics &statistics) {
    return statistics.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, Statistics &statistics) {
    return statistics.read(stream);
  }
} // end namespace isis
