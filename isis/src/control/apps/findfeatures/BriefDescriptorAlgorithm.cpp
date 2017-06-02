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

#include "BriefDescriptorAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  BriefDescriptorAlgorithm::BriefDescriptorAlgorithm() : 
                 Feature2DAlgorithm("Brief", "Feature2D",
                                    BriefType::create()) {  
    setupParameters();
  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  BriefDescriptorAlgorithm::BriefDescriptorAlgorithm(const PvlFlatMap &cvars, 
                                                     const QString &config) :
                           Feature2DAlgorithm("Brief", "Feature2D", 
                                               BriefType::create(), cvars) {
    setConfig(config);
    PvlFlatMap variables = setupParameters();
    variables.merge(cvars);
    int bytes = toInt(variables.get("Bytes"));
    bool useOrientation = toBool(variables.get("UseOrientation"));

    m_algorithm = BriefType::create(bytes, useOrientation);

    m_variables.merge(variables);
  }


  /**
   * Destroys the algorithm
   */
  BriefDescriptorAlgorithm::~BriefDescriptorAlgorithm() { }


  /**
   * Sets up the algorithm parameters with default values. 
   * 
   * @return PvlFlatMap Algorithm parameters and their default values. 
   */
  PvlFlatMap BriefDescriptorAlgorithm::setupParameters() {
    PvlFlatMap variables;
    variables.add("Bytes",          "32");
    variables.add("UseOrientation", "true");
    m_variables = variables;
    return (m_variables);
  }


  /**
   * Returns a description of the algorithm.
   * 
   * @return @b QString A description of the algorithm.
   */
  QString BriefDescriptorAlgorithm::description() const {
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
  Feature2DAlgorithm *BriefDescriptorAlgorithm::create(const PvlFlatMap &vars,
                                                       const QString &config) {

    return ( new BriefDescriptorAlgorithm(vars, config) );
  }



  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool BriefDescriptorAlgorithm::hasDetector() const { 
    return false; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool BriefDescriptorAlgorithm::hasExtractor() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool BriefDescriptorAlgorithm::hasMatcher() const { 
    return false; 
  }


  /**
   * Returns the variables and their values used by the algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap BriefDescriptorAlgorithm::getAlgorithmVariables( ) const{
    return ( variables() );
  }


  /**
   * @brief Set parameters as provided by the variables
   * 
   * @param variables Container of parameters to set
   * 
   * @return @b int Always -1, variables cannot be set after initialization.
   * 
   * @throws IException::Programmer "BriefDescriptorAlgorithm does not have the ability
   *                                 to set algorithm parameters."
   */
  int BriefDescriptorAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {
    QString msg = "BriefDescriptorAlgorithm does not have the ability to set algorithm parameters.";
    throw IException(IException::Programmer, msg, _FILEINFO_);

    return (-1);
  }

};  // namespace Isis
