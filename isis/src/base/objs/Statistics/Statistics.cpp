/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Statistics.h"

#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QUuid>
#include <QXmlStreamWriter>

#include <float.h>

#include "IException.h"
#include "IString.h"
#include "Project.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "XmlStackedHandlerReader.h"

using namespace std;

namespace Isis {
  //! Constructs an IsisStats object with accumulators and counters set to zero.
  Statistics::Statistics(QObject *parent) : QObject(parent) {
//    m_id = NULL;
//    m_id = new QUuid(QUuid::createUuid());
    SetValidRange();
    Reset(); // initialize
  }


  Statistics::Statistics(Project *project, XmlStackedHandlerReader *xmlReader, QObject *parent) {   // TODO: does xml stuff need project???
//    m_id = NULL;
    SetValidRange();
    Reset(); // initialize
    xmlReader->pushContentHandler(new XmlHandler(this, project));   // TODO: does xml stuff need project???
  }

  /**
   * Constructs a Statistics object from an input Pvl
   *
   * @param const PvlGroup & - The input statistics
   */
  Statistics::Statistics(const PvlGroup &inStats, QObject *parent) {
    SetValidRange();
    Reset();
    fromPvl(inStats);
  }


  Statistics::Statistics(const Statistics &other)
    : m_sum(other.m_sum),
      m_sumsum(other.m_sumsum),
      m_minimum(other.m_minimum),
      m_maximum(other.m_maximum),
      m_validMinimum(other.m_validMinimum),
      m_validMaximum(other.m_validMaximum),
      m_totalPixels(other.m_totalPixels),
      m_validPixels(other.m_validPixels),
      m_nullPixels(other.m_nullPixels),
      m_lrsPixels(other.m_lrsPixels),
      m_lisPixels(other.m_lisPixels),
      m_hrsPixels(other.m_hrsPixels),
      m_hisPixels(other.m_hisPixels),
      m_underRangePixels(other.m_underRangePixels),
      m_overRangePixels(other.m_overRangePixels),
      m_removedData(other.m_removedData) {
  }
   // : m_id(new QUuid(other.m_id->toString())),


  //! Destroys the IsisStats object.
  Statistics::~Statistics() {
//    delete m_id;
//    m_id = NULL;
  }


  Statistics &Statistics::operator=(const Statistics &other) {

    if (&other != this) {
//      delete m_id;
//      m_id = NULL;
//      m_id = new QUuid(other.m_id->toString());

      m_sum = other.m_sum;
      m_sumsum = other.m_sumsum;
      m_minimum = other.m_minimum;
      m_maximum = other.m_maximum;
      m_validMinimum = other.m_validMinimum;
      m_validMaximum = other.m_validMaximum;
      m_totalPixels = other.m_totalPixels;
      m_validPixels = other.m_validPixels;
      m_nullPixels = other.m_nullPixels;
      m_lrsPixels = other.m_lrsPixels;
      m_lisPixels = other.m_lisPixels;
      m_hrsPixels = other.m_hrsPixels;
      m_hisPixels = other.m_hisPixels;
      m_underRangePixels = other.m_underRangePixels;
      m_overRangePixels = other.m_overRangePixels;
      m_removedData = other.m_removedData;
    }
    return *this;

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

    if (Isis::IsNullPixel(data)) {
      m_nullPixels++;
    }
    else if (Isis::IsHisPixel(data)) {
      m_hisPixels++;
    }
    else if (Isis::IsHrsPixel(data)) {
      m_hrsPixels++;
    }
    else if (Isis::IsLisPixel(data)) {
      m_lisPixels++;
    }
    else if (Isis::IsLrsPixel(data)) {
      m_lrsPixels++;
    }
    else if (AboveRange(data)) {
      m_overRangePixels++;
    }
    else if (BelowRange(data)) {
      m_underRangePixels++;
    }
    else { // if (Isis::IsValidPixel(data) && InRange(data)) {
      m_sum += data;
      m_sumsum += data * data;
      if (data < m_minimum) m_minimum = data;
      if (data > m_maximum) m_maximum = data;
      m_validPixels++;
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

    for(unsigned int i = 0; i < count; i++) {
      double value = data[i];
      RemoveData(value);
    }

  }


  void Statistics::RemoveData(const double data) {
    m_removedData = true;
    m_totalPixels--;

    if (Isis::IsNullPixel(data)) {
      m_nullPixels--;
    }
    else if (Isis::IsHisPixel(data)) {
      m_hisPixels--;
    }
    else if (Isis::IsHrsPixel(data)) {
      m_hrsPixels--;
    }
    else if (Isis::IsLisPixel(data)) {
      m_lisPixels--;
    }
    else if (Isis::IsLrsPixel(data)) {
      m_lrsPixels--;
    }
    else if (AboveRange(data)) {
      m_overRangePixels--;
    }
    else if (BelowRange(data)) {
      m_underRangePixels--;
    }
    else { // if (IsValidPixel(data) && InRange(data)) {
      m_sum -= data;
      m_sumsum -= data * data;
      m_validPixels--;
    }

    if (m_totalPixels < 0) {
      QString msg = "You are removing non-existant data in [Statistics::RemoveData]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    // what happens to saved off min/max???
  }


  void Statistics::SetValidRange(const double minimum, const double maximum) {
    m_validMinimum = minimum;
    m_validMaximum = maximum;

    if (m_validMaximum < m_validMinimum) {
      // get the min and max DN values in the chosen range
      QString msg = "Invalid Range: Minimum [" + toString(minimum) 
                    + "] must be less than the Maximum [" + toString(maximum) + "].";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    //??? throw exception if data has already been added???
  }


  double Statistics::ValidMinimum() const {
    return m_validMinimum;
  }


  double Statistics::ValidMaximum() const {
    return m_validMaximum;
  }


  bool Statistics::InRange(const double value) {
    return (!BelowRange(value) && !AboveRange(value));
  }


  bool Statistics::AboveRange(const double value) {
    return (value > m_validMaximum);
  }


  bool Statistics::BelowRange(const double value) {
    return (value < m_validMinimum);
  }


  /**
   * Computes and returns the average.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return The Average
   */
  double Statistics::Average() const {
    if (m_validPixels < 1) return Isis::NULL8;
    return m_sum / m_validPixels;
  }


  /**
   * Computes and returns the standard deviation.
   * If there are no valid pixels, then NULL8 is returned.
   *
   * @return The standard deviation
   */
  double Statistics::StandardDeviation() const {
    if (m_validPixels <= 1) return Isis::NULL8;
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
    if (m_validPixels <= 1) return Isis::NULL8;
    double temp = m_validPixels * m_sumsum - m_sum * m_sum;
    if (temp < 0.0) temp = 0.0;  // This should happen unless roundoff occurs
    return temp / ((m_validPixels - 1.0) * m_validPixels);
  }


  /**
   * Returns the sum of all the data
   *
   * @return The sum of the data
   */
  double Statistics::Sum() const {
    return m_sum;
  }


  /**
   * Returns the sum of all the squared data
   *
   * @return The sum of the squared data
   */
  double Statistics::SumSquare() const {
    return m_sumsum;
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
    if (m_validPixels < 1) return Isis::NULL8;
    double temp = m_sumsum / m_validPixels;
    if (temp < 0.0) temp = 0.0;
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
    if (m_removedData) {
      QString msg = "Minimum is invalid since you removed data";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_validPixels < 1) return Isis::NULL8;
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
    if (m_removedData) {
      QString msg = "Maximum is invalid since you removed data";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_validPixels < 1) return Isis::NULL8;
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
    if ((percent <= 0.0) || (percent >= 100.0)) {
      QString msg = "Invalid value for percent";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_validPixels < 1) return Isis::NULL8;
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
    if ((percent <= 0.0) || (percent >= 100.0)) {
      QString msg = "Invalid value for percent";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_validPixels < 1) return Isis::NULL8;
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
    if (m_validPixels < 1) return Isis::NULL8;
    double min = ChebyshevMinimum(percent);
    if (Minimum() > min) min = Minimum();
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
    if (m_validPixels < 1) return Isis::NULL8;
    double max = ChebyshevMaximum(percent);
    if (Maximum() < max) max = Maximum();
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
    if (StandardDeviation() == 0) {
      if (value == Maximum()) return 0;
      else {
        QString msg = "Undefined Z-score. Standard deviation is zero and the input value[" 
                      + toString(value) + "] is out of range [" + toString(Maximum()) + "].";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    return (value - Average()) / StandardDeviation();
  }


  /**
   * Unserializes a Statistics object from a pvl group
   *
   * @param const PvlGroup &inStats - The input statistics
   */
  void Statistics::fromPvl(const PvlGroup &inStats) {
    Reset();
    m_sum = inStats["Sum"];
    m_sumsum = inStats["SumSquare"];
    m_minimum = inStats["Minimum"];
    m_maximum = inStats["Maximum"];
    m_validMinimum = inStats["ValidMinimum"];
    m_validMaximum = inStats["ValidMaximum"];
    m_totalPixels = inStats["TotalPixels"];
    m_validPixels = inStats["ValidPixels"];
    m_nullPixels = inStats["NullPixels"];
    m_lrsPixels = inStats["LrsPixels"];
    m_lisPixels = inStats["LisPixels"];
    m_hrsPixels = inStats["HrsPixels"];
    m_hisPixels = inStats["HisPixels"];
    m_underRangePixels = inStats["UnderValidMinimumPixels"];
    m_overRangePixels = inStats["OverValidMaximumPixels"];
    m_removedData = false; //< Is this the correct state?
  }


  /**
   * Serialize statistics as a pvl group
   *
   * @param QString name (Default value is "Statistics") - Name of the statistics group
   *
   * @return PvlGroup Statistics information as a pvl group
   */
  PvlGroup Statistics::toPvl(QString name) const {
    if (name.isEmpty()) {
      name = "Statistics";
    }
    // Construct a label with the results
    PvlGroup results(name);  
    results += PvlKeyword("Sum", toString(Sum()));
    results += PvlKeyword("SumSquare", toString(SumSquare()));
    results += PvlKeyword("Minimum", toString(Minimum()));
    results += PvlKeyword("Maximum", toString(Maximum()));
    results += PvlKeyword("ValidMinimum", toString(ValidMinimum()));
    results += PvlKeyword("ValidMaximum", toString(ValidMaximum()));
    if (ValidPixels() != 0) {
      results += PvlKeyword("Average", toString(Average()));
      results += PvlKeyword("StandardDeviation", toString(StandardDeviation()));
      results += PvlKeyword("Variance", toString(Variance()));
    }
    results += PvlKeyword("TotalPixels", toString(TotalPixels()));
    results += PvlKeyword("ValidPixels", toString(ValidPixels()));
    results += PvlKeyword("OverValidMaximumPixels", toString(OverRangePixels()));
    results += PvlKeyword("UnderValidMinimumPixels", toString(UnderRangePixels()));
    results += PvlKeyword("NullPixels", toString(NullPixels()));
    results += PvlKeyword("LisPixels", toString(LisPixels()));
    results += PvlKeyword("LrsPixels", toString(LrsPixels()));
    results += PvlKeyword("HisPixels", toString(HisPixels()));
    results += PvlKeyword("HrsPixels", toString(HrsPixels()));

    return results;
  }


  void Statistics::save(QXmlStreamWriter &stream, const Project *project) const {   // TODO: does xml stuff need project???

    stream.writeStartElement("statistics");
//    stream.writeTextElement("id", m_id->toString());
 
    stream.writeTextElement("sum", toString(m_sum));
    stream.writeTextElement("sumSquares", toString(m_sumsum));

    stream.writeStartElement("range");
    stream.writeTextElement("minimum", toString(m_minimum));
    stream.writeTextElement("maximum", toString(m_maximum));
    stream.writeTextElement("validMinimum", toString(m_validMinimum));
    stream.writeTextElement("validMaximum", toString(m_validMaximum));
    stream.writeEndElement(); // end range
    
    stream.writeStartElement("pixelCounts");
    stream.writeTextElement("totalPixels", toString(m_totalPixels));
    stream.writeTextElement("validPixels", toString(m_validPixels));
    stream.writeTextElement("nullPixels", toString(m_nullPixels));
    stream.writeTextElement("lisPixels", toString(m_lisPixels));
    stream.writeTextElement("lrsPixels", toString(m_lrsPixels));
    stream.writeTextElement("hisPixels", toString(m_hisPixels));
    stream.writeTextElement("hrsPixels", toString(m_hrsPixels));
    stream.writeTextElement("underRangePixels", toString(m_underRangePixels));
    stream.writeTextElement("overRangePixels", toString(m_overRangePixels));
    stream.writeEndElement(); // end pixelCounts
    
    stream.writeTextElement("removedData", toString(m_removedData));
    stream.writeEndElement(); // end statistics

  }


  Statistics::XmlHandler::XmlHandler(Statistics *statistics, Project *project) {   // TODO: does xml stuff need project???
    m_xmlHandlerStatistics = statistics;
    m_xmlHandlerProject = project;   // TODO: does xml stuff need project???
    m_xmlHandlerCharacters = "";
  }


  Statistics::XmlHandler::~XmlHandler() {
    // do not delete this pointer... we don't own it, do we??? passed into StatCumProbDistDynCalc constructor as pointer
    // delete m_xmlHandlerProject;    // TODO: does xml stuff need project???
    m_xmlHandlerProject = NULL;
  }


  bool Statistics::XmlHandler::startElement(const QString &namespaceURI, 
                                                                const QString &localName,
                                                                const QString &qName,
                                                                const QXmlAttributes &atts) {
    m_xmlHandlerCharacters = "";
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      // no element attibutes to evaluate
    }
    return true;
  }


  bool Statistics::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }


  bool Statistics::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName) {
    if (!m_xmlHandlerCharacters.isEmpty()) {
      if (localName == "id") {
//        m_xmlHandlerStatistics->m_id = NULL;
//        m_xmlHandlerStatistics->m_id = new QUuid(m_xmlHandlerCharacters);
      }
      if (localName == "sum") {
        m_xmlHandlerStatistics->m_sum = toDouble(m_xmlHandlerCharacters);
      }
      if (localName == "sumSquares") {
        m_xmlHandlerStatistics->m_sumsum = toDouble(m_xmlHandlerCharacters);
      }
      if (localName == "minimum") {
        m_xmlHandlerStatistics->m_minimum = toDouble(m_xmlHandlerCharacters);
      }
      if (localName == "maximum") {
        m_xmlHandlerStatistics->m_maximum = toDouble(m_xmlHandlerCharacters);
      }
      if (localName == "validMinimum") {
        m_xmlHandlerStatistics->m_validMinimum = toDouble(m_xmlHandlerCharacters);
      }
      if (localName == "validMaximum") {
        m_xmlHandlerStatistics->m_validMaximum = toDouble(m_xmlHandlerCharacters);
      }
      if (localName == "totalPixels") {
        m_xmlHandlerStatistics->m_totalPixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "validPixels") {
        m_xmlHandlerStatistics->m_validPixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "nullPixels") {
        m_xmlHandlerStatistics->m_nullPixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "lisPixels") {
        m_xmlHandlerStatistics->m_lisPixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "lrsPixels") {
        m_xmlHandlerStatistics->m_lrsPixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "hisPixels") {
        m_xmlHandlerStatistics->m_hisPixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "hrsPixels") {
        m_xmlHandlerStatistics->m_hrsPixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "underRangePixels") {
        m_xmlHandlerStatistics->m_underRangePixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "overRangePixels") {
        m_xmlHandlerStatistics->m_overRangePixels = toBigInt(m_xmlHandlerCharacters);
      }
      if (localName == "removedData") {
        m_xmlHandlerStatistics->m_removedData = toBool(m_xmlHandlerCharacters);
      }
      m_xmlHandlerCharacters = "";
    }
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }


  /** 
   * Order saved must match the offsets in the static compoundH5DataType() 
   * method. 
   */ 
  QDataStream &Statistics::write(QDataStream &stream) const {
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << m_sum
           << m_sumsum
           << m_minimum
           << m_maximum
           << m_validMinimum
           << m_validMaximum
           << (qint64)m_totalPixels
           << (qint64)m_validPixels
           << (qint64)m_nullPixels
           << (qint64)m_lrsPixels
           << (qint64)m_lisPixels
           << (qint64)m_hrsPixels
           << (qint64)m_hisPixels
           << (qint64)m_underRangePixels
           << (qint64)m_overRangePixels
           << (qint32)m_removedData;
    return stream;
//    stream << m_id->toString()
  }


  QDataStream &Statistics::read(QDataStream &stream) {

//    QString id;
    qint64 totalPixels, validPixels, nullPixels, lrsPixels, lisPixels,
           hrsPixels, hisPixels, underRangePixels, overRangePixels;
    qint32 removedData;

    stream.setByteOrder(QDataStream::LittleEndian);
//    stream >> id
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
           >> removedData;

//    delete m_id;
//    m_id = NULL;
//    m_id = new QUuid(id);

    m_totalPixels      = (BigInt)totalPixels;
    m_validPixels      = (BigInt)validPixels;
    m_nullPixels       = (BigInt)nullPixels;
    m_lrsPixels        = (BigInt)lrsPixels;
    m_lisPixels        = (BigInt)lisPixels;
    m_hrsPixels        = (BigInt)hrsPixels;
    m_hisPixels        = (BigInt)hisPixels;
    m_underRangePixels = (BigInt)underRangePixels;
    m_overRangePixels  = (BigInt)overRangePixels;
    m_removedData      = (bool)removedData;
    
    return stream;
  }


  QDataStream &operator<<(QDataStream &stream, const Statistics &statistics) {
    return statistics.write(stream);
  }


  QDataStream &operator>>(QDataStream &stream, Statistics &statistics) {
    return statistics.read(stream);
  }

} // end namespace isis
