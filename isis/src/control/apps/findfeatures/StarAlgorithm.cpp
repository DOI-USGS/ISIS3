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
#include "opencv2/xfeatures2d.hpp"

#include "StarAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  StarAlgorithm::StarAlgorithm() : 
                 Feature2DAlgorithm("Star", "Feature2D",
                                    StarType::create()) {
    setupParameters();
  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  StarAlgorithm::StarAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                 Feature2DAlgorithm("Star", "Feature2D", 
                                     StarType::create(), cvars) {
    setConfig(config);
    PvlFlatMap variables = setupParameters();
    variables.merge(cvars);
    const int maxSize = toInt(variables.get("MaxSize"));
    const int responseThreshold = toInt(variables.get("ResponseThreshold"));
    const int lineThresholdProjected = toInt(variables.get("LineThresholdProjected"));
    const int lineThresholdBinarized = toInt(variables.get("LineThresholdBinarized"));
    const int SuppressNonmaxSize = toInt(variables.get("SuppressNonmaxSize"));

    m_algorithm = StarType::create(maxSize, responseThreshold, lineThresholdProjected, 
                                   lineThresholdBinarized, SuppressNonmaxSize);

    m_variables.merge(variables);
  }


  /**
   * Destroys the algorithm
   */
  StarAlgorithm::~StarAlgorithm() { }


  /**
   * Sets up the algorithm parameters with default values. 
   * 
   * @return PvlFlatMap Algorithm parameters and their default values. 
   */
  PvlFlatMap StarAlgorithm::setupParameters() {
    PvlFlatMap variables;
    variables.add("MaxSize",                "45");
    variables.add("ResponseThreshold",      "30");
    variables.add("LineThresholdProjected", "10");
    variables.add("LineThresholdBinarized", "8");
    variables.add("SuppressNonmaxSize",     "5");
    m_variables = variables;
    return (m_variables);
  }


  /**
   * Returns a description of the algorithm.
   * 
   * @return @b QString A description of the algorithm.
   */
  QString StarAlgorithm::description() const {
    QString desc = "The OpenCV Star Feature2D detector/extractor algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/d6/d36/classcv_1_1xfeatures2d_1_1Star.html";
    return (desc);
  }


  /**
   * Creates an instance of the algorithm.
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *StarAlgorithm::create(const PvlFlatMap &vars, const QString &config) {

    return ( new StarAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool StarAlgorithm::hasDetector() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool StarAlgorithm::hasExtractor() const { 
    return false; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool StarAlgorithm::hasMatcher() const { 
    return false; 
  }


  /**
   * Returns the variables and their values used by the algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap StarAlgorithm::getAlgorithmVariables( ) const {
    return ( variables() );
  }


 /**
  * @brief Set parameters as provided by the variables
  * 
  * @param variables Container of parameters to set
  * 
  * @return @b int Always -1, variables cannot be set after initialization.
  * 
  * @throws IException::Programmer "StarAlgorithm does not have the ability
  *                                 to set algorithm parameters."
  */
  int StarAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {
    QString msg = "StarAlgorithm does not have the ability to set algorithm parameters.";
    throw IException(IException::Programmer, msg, _FILEINFO_);

    return (-1);
  }

};  // namespace Isis
