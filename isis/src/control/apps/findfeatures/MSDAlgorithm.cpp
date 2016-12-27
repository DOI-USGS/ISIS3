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

#include "MSDAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  MSDAlgorithm::MSDAlgorithm() : 
                 Feature2DAlgorithm("MSD", "Feature2D",
                                    MSDType::create()) {  
    setupParameters();

  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  MSDAlgorithm::MSDAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                Feature2DAlgorithm("MSD", "Feature2D", 
                                    MSDType::create(), cvars) {
    setConfig(config);
    PvlFlatMap variables = setupParameters();
    variables.merge(cvars);
    const int patchRadius = toInt(variables.get("PatchRadius"));
    const int searchAreaRadius = toInt(variables.get("SearchAreaRadius"));
    const int nmsRadius = toInt(variables.get("NMSRadius"));
    const int nmsScaleRadius = toInt(variables.get("NMSScaleRadius"));
    const float thSaliency = variables.get("THSaliency").toFloat();
    const int kNN = toInt(variables.get("KNN"));
    const float scaleFactor = variables.get("ScaleFactor").toFloat();
    const int nScales = toInt(variables.get("NScales"));
    const bool computeOrientation = toBool(variables.get("ComputeOrientation"));

    m_algorithm = MSDType::create(patchRadius, searchAreaRadius, nmsRadius, nmsScaleRadius, 
                                  thSaliency, kNN, scaleFactor, nScales, computeOrientation);
                                  
    m_variables.merge(variables);
  }


  /**
   * Destroys the algorithm
   */
  MSDAlgorithm::~MSDAlgorithm() { }


  /**
   * Sets up the algorithm parameters with default values. 
   * 
   * @return PvlFlatMap Algorithm parameters and their default values. 
   */
  PvlFlatMap MSDAlgorithm::setupParameters() {
    PvlFlatMap variables;
    variables.add("PatchRadius",        "3");
    variables.add("SearchAreaRadius",   "5");
    variables.add("NMSRadius",          "5");
    variables.add("NMSScaleRadius",     "0");
    variables.add("THSaliency",         "250.0");
    variables.add("KNN",                "4");
    variables.add("ScaleFactor",        "1.25");
    variables.add("NScales",            "-1");
    variables.add("ComputeOrientation", "false");
    m_variables = variables;
    return (m_variables);
  }


  /**
   * Returns a description of the algorithm.
   * 
   * @return @b QString A description of the algorithm.
   */
  QString MSDAlgorithm::description() const {
    QString desc = "The OpenCV MSD Feature2D detector/extractor algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/d6/d36/classcv_1_1xfeatures2d_1_1MSD.html";
    return (desc);
  }


  /**
   * Creates an instance of the algorithm.
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *MSDAlgorithm::create(const PvlFlatMap &vars, const QString &config) {

    return ( new MSDAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool MSDAlgorithm::hasDetector() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool MSDAlgorithm::hasExtractor() const { 
    return false; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool MSDAlgorithm::hasMatcher() const { 
    return false; 
  }


  /**
   * Returns the variables and their values used by the algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap MSDAlgorithm::getAlgorithmVariables( ) const {
    return (variables());
  }


/**
 * @brief Set parameters as provided by the variables
 * 
 * @param variables Container of parameters to set
 * 
 * @return @b int Always -1, variables cannot be set after initialization.
 * 
 * @throws IException::Programmer "MSDAlgorithm does not have the ability
 *                                 to set algorithm parameters."
 */
  int MSDAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {
    QString msg = "MSDAlgorithm does not have the ability to set algorithm parameters.";
    throw IException(IException::Programmer, msg, _FILEINFO_);

    return (-1);
  }

};  // namespace Isis
