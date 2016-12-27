/**
 * @file
 * $Revision$ 
 * $Date$ 
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
    m_variables.merge(variables);
  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  BlobDetectionAlgorithm::BlobDetectionAlgorithm(const PvlFlatMap &cvars, const QString &config,
                                                 const cv::SimpleBlobDetector::Params params) :
                  Feature2DAlgorithm("Blob", "Feature2D", 
                                     BLOBType::create(params), cvars) {
    setConfig(config);
    PvlFlatMap variables;
    variables.add("ThresholdStep",       toString(params.thresholdStep));
    variables.add("MinThreshold",        toString(params.minThreshold));
    variables.add("MaxThreshold",        toString(params.maxThreshold));
    variables.add("MinRepeatability",    toString( (int) params.minRepeatability));
    variables.add("MinDistance",         toString(params.minDistBetweenBlobs));
    variables.add("FilterByColor",       toString(params.filterByColor));
    variables.add("BlobColor",           toString(params.blobColor));
    variables.add("FilterByArea",        toString(params.filterByArea));
    variables.add("MinArea",             toString(params.minArea));
    variables.add("MaxArea",             toString(params.maxArea));
    variables.add("FilterByCircularity", toString(params.filterByCircularity));
    variables.add("MinCircularity",      toString(params.minCircularity));
    variables.add("maxCircularity",      toString(params.maxCircularity));
    variables.add("FilterByInertia",     toString(params.filterByInertia));
    variables.add("MinInertiaRatio",     toString(params.minInertiaRatio));
    variables.add("MaxInertiaRatio",     toString(params.maxInertiaRatio));
    variables.add("FilterByConvexity",   toString(params.filterByConvexity));
    variables.add("MinConvexity",        toString(params.minConvexity));
    variables.add("MaxConvexity",        toString(params.maxConvexity));
    m_variables.merge(variables);
  }


  /**
   * Destroys the algorithm
   */
  BlobDetectionAlgorithm::~BlobDetectionAlgorithm() { }


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
    cv::SimpleBlobDetector::Params params;
    params.thresholdStep = vars.get("ThresholdStep", "10").toFloat();
    params.minThreshold = vars.get("MinThreshold", "50").toFloat();
    params.maxThreshold = vars.get("MaxThreshold", "220").toFloat();
    params.minRepeatability = vars.get("MinRepeatability", "2").toUInt();
    params.minDistBetweenBlobs = vars.get("MinDistance", "10").toFloat();
    params.filterByColor = toBool(vars.get("FilterByColor", "true"));
    params.blobColor = (unsigned char) toInt(vars.get("BlobColor", "0"));
    params.filterByArea = toBool(vars.get("FilterByArea", "true"));
    params.minArea = vars.get("MinArea", "25").toFloat();
    params.maxArea = vars.get("MaxArea", "5000").toFloat();
    params.filterByCircularity = toBool(vars.get("FilterByCircularity", "false"));
    params.minCircularity = vars.get("MinCircularity", "0.8").toFloat();
    params.maxCircularity = vars.get("maxCircularity", "inf").toFloat();
    params.filterByInertia = toBool(vars.get("FilterByInertia", "true"));
    params.minInertiaRatio = vars.get("MinInertiaRatio", "0.1").toFloat();
    params.maxInertiaRatio = vars.get("MaxInertiaRatio", "inf").toFloat();
    params.filterByConvexity = toBool(vars.get("FilterByConvexity", "true"));
    params.minConvexity = vars.get("MinConvexity", "0.95").toFloat();
    params.maxConvexity = vars.get("MaxConvexity", "inf").toFloat();
    return ( new BlobDetectionAlgorithm(vars, config, params) );
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
