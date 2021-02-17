/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <boost/foreach.hpp>
#include "opencv2/opencv.hpp"

#include "BlobDetectionAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  BlobDetectionAlgorithm::BlobDetectionAlgorithm() :
                 Feature2DAlgorithm("Blob", "Feature2D",
                                    BLOBType::create()) {
   setupParameters();
  }


  /**
   * Constructs the algorithm with the input variables
   *
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  BlobDetectionAlgorithm::BlobDetectionAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                          Feature2DAlgorithm("Blob", "Feature2D",
                                              BLOBType::create(), cvars) {
    setConfig(config);
    PvlFlatMap variables = setupParameters();
    variables.merge(cvars);
    cv::SimpleBlobDetector::Params params;
    params.thresholdStep = variables.get("ThresholdStep").toFloat();
    params.minThreshold = variables.get("MinThreshold").toFloat();
    params.maxThreshold = variables.get("MaxThreshold").toFloat();
    params.minRepeatability = variables.get("MinRepeatability").toUInt();
    params.minDistBetweenBlobs = variables.get("MinDistance").toFloat();
    params.filterByColor = toBool(variables.get("FilterByColor"));
    params.blobColor = (unsigned char) toInt(variables.get("BlobColor"));
    params.filterByArea = toBool(variables.get("FilterByArea"));
    params.minArea = variables.get("MinArea").toFloat();
    params.maxArea = variables.get("MaxArea").toFloat();
    params.filterByCircularity = toBool(variables.get("FilterByCircularity"));
    params.minCircularity = variables.get("MinCircularity").toFloat();
    params.maxCircularity = variables.get("maxCircularity").toFloat();
    params.filterByInertia = toBool(variables.get("FilterByInertia"));
    params.minInertiaRatio = variables.get("MinInertiaRatio").toFloat();
    params.maxInertiaRatio = variables.get("MaxInertiaRatio").toFloat();
    params.filterByConvexity = toBool(variables.get("FilterByConvexity"));
    params.minConvexity = variables.get("MinConvexity").toFloat();
    params.maxConvexity = variables.get("MaxConvexity").toFloat();

    m_algorithm = BLOBType::create(params);

    m_variables.merge(variables);
  }


  /**
   * Destroys the algorithm
   */
  BlobDetectionAlgorithm::~BlobDetectionAlgorithm() { }


  /**
   * Sets up the algorithm parameters with default values.
   *
   * @return PvlFlatMap Algorithm parameters and their default values.
   */
  PvlFlatMap BlobDetectionAlgorithm::setupParameters() {
    PvlFlatMap variables;
    variables.add("ThresholdStep",       "10");
    variables.add("MinThreshold",        "50");
    variables.add("MaxThreshold",        "220");
    variables.add("MinRepeatability",    "2");
    variables.add("MinDistance",         "10");
    variables.add("FilterByColor",       "true");
    variables.add("BlobColor",           "0");
    variables.add("FilterByArea",        "true");
    variables.add("MinArea",             "25");
    variables.add("MaxArea",             "5000");
    variables.add("FilterByCircularity", "false");
    variables.add("MinCircularity",      "0.8");
    variables.add("maxCircularity",      "inf");
    variables.add("FilterByInertia",     "true");
    variables.add("MinInertiaRatio",     "0.1");
    variables.add("MaxInertiaRatio",     "inf");
    variables.add("FilterByConvexity",   "true");
    variables.add("MinConvexity",        "0.95");
    variables.add("MaxConvexity",        "inf");
    m_variables = variables;
    return (m_variables);
  }


  /**
   * Returns a description of the algorithm.
   *
   * @return @b QString A description of the algorithm.
   */
  QString BlobDetectionAlgorithm::description() const {
    QString desc = "The OpenCV simple blob detection algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/d0/d7a/classcv_1_1SimpleBlobDetector.html";
    return (desc);
  }


  /**
   * Creates an instance of the algorithm.
   *
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *BlobDetectionAlgorithm::create(const PvlFlatMap &vars,
                                                     const QString &config) {
    return ( new BlobDetectionAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector.
   *
   * @return @b true if the algorithm has a detector.
   */
  bool BlobDetectionAlgorithm::hasDetector() const {
    return true;
  }


  /**
   * Returns true if the algorithm has an extractor.
   *
   * @return @b true if the algorithm has an extractor.
   */
  bool BlobDetectionAlgorithm::hasExtractor() const {
    return false;
  }


  /**
   * Returns true if the algorithm has a matcher.
   *
   * @return @b true if the algorithm has a matcher.
   */
  bool BlobDetectionAlgorithm::hasMatcher() const {
    return false;
  }


  /**
   * Returns the variables and their values used by the algorithm.
   *
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap BlobDetectionAlgorithm::getAlgorithmVariables( ) const{
    return ( variables() );
  }


/**
 * @brief Set parameters as provided by the variables
 *
 * @param variables Container of parameters to set
 *
 * @return @b int Always -1, variables cannot be set after initialization.
 *
 * @throws IException::Programmer "BlobDetectionAlgorithm does not have the ability
 *                                 to set algorithm parameters."
 */
  int BlobDetectionAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {
    QString msg = "BlobDetectionAlgorithm does not have the ability to set algorithm parameters.";
    throw IException(IException::Programmer, msg, _FILEINFO_);

    return (-1);
  }

};  // namespace Isis
