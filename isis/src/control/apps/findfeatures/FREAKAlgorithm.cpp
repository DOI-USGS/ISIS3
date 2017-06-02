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

#include "FREAKAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  FREAKAlgorithm::FREAKAlgorithm() : 
                 Feature2DAlgorithm("FREAK", "Feature2D",
                                    FREAKType::create()) {  
    setupParameters();
  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  FREAKAlgorithm::FREAKAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                  Feature2DAlgorithm("FREAK", "Feature2D", 
                                     FREAKType::create(), cvars) {
    setConfig(config);
    PvlFlatMap variables = setupParameters();
    variables.merge(cvars);
    const bool orientationNormalized = toBool(variables.get("OrientationNormalized"));
    const bool scaleNormalized = toBool(variables.get("ScaleNormalized"));
    const float patternScale = variables.get("PatternScale").toFloat();
    const int nOctaves = toInt(variables.get("NOctaves"));
    std::vector<int> selectedPairs;
    if (!variables.get("SelectedPairs").isEmpty()) {
      QStringList pairList = variables.get("SelectedPairs").split(",");
      BOOST_FOREACH(QString pair, pairList) {
        selectedPairs.push_back(toInt(pair));
      }
    }

    m_algorithm = FREAKType::create(orientationNormalized, scaleNormalized, patternScale, 
                                    nOctaves, selectedPairs);

    m_variables.merge(variables);
  }


 /**
   * Sets up the algorithm parameters with default values. 
   * 
   * @return PvlFlatMap Algorithm parameters and their default values. 
   */
  PvlFlatMap FREAKAlgorithm::setupParameters() {
    PvlFlatMap variables;
    variables.add("OrientationNormalized", "true");
    variables.add("ScaleNormalized",       "true");
    variables.add("PatternScale",          "22.0");
    variables.add("NOctaves",              "4");
    variables.add("SelectedPairs",         "");
    m_variables = variables;
    return (m_variables);
  }


  /**
   * Destroys the algorithm
   */
  FREAKAlgorithm::~FREAKAlgorithm() { }


  /**
   * Returns a description of the algorithm.
   * 
   * @return @b QString A description of the algorithm.
   */
  QString FREAKAlgorithm::description() const {
    QString desc = "The OpenCV FREAK Feature2D detector/extractor algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/df/db4/classcv_1_1xfeatures2d_1_1FREAK.html";
    return (desc);
  }


  /**
   * Creates an instance of the algorithm.
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *FREAKAlgorithm::create(const PvlFlatMap &vars, const QString &config) {
    return ( new FREAKAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool FREAKAlgorithm::hasDetector() const { 
    return false; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool FREAKAlgorithm::hasExtractor() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool FREAKAlgorithm::hasMatcher() const { 
    return false; 
  }


  /**
   * Returns the variables and their values used by the algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap FREAKAlgorithm::getAlgorithmVariables( ) const {
    return ( variables() );
  }


  /**
   * @brief Set parameters as provided by the variables
   * 
   * @param variables Container of parameters to set
   * 
   * @return @b int Always -1, variables cannot be set after initialization.
   * 
   * @throws IException::Programmer "FREAKAlgorithm does not have the ability
   *                                 to set algorithm parameters."
   */
  int FREAKAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {
    QString msg = "FREAKAlgorithm does not have the ability to set algorithm parameters.";
    throw IException(IException::Programmer, msg, _FILEINFO_);

    return (-1);
  }

};  // namespace Isis
