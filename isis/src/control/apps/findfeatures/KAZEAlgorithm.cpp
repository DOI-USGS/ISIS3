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

#include "KAZEAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  KAZEAlgorithm::KAZEAlgorithm() : 
                 Feature2DAlgorithm("KAZE", "Feature2D",
                                    KAZEType::create()) {  
    setupTypeMap();
    m_variables.merge( getAlgorithmVariables() );
  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  KAZEAlgorithm::KAZEAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                  Feature2DAlgorithm("KAZE", "Feature2D", 
                                     KAZEType::create(), cvars) {
    setupTypeMap();
    setConfig(config);
    setAlgorithmVariables(cvars);
    m_variables.merge(getAlgorithmVariables());
  }


  /**
   * Destroys the algorithm
   */
  KAZEAlgorithm::~KAZEAlgorithm() { }


  /**
   * Returns a description of the algorithm.
   * 
   * @return @b QString A description of the algorithm.
   */
  QString KAZEAlgorithm::description() const {
    QString desc = "The OpenCV KAZE Feature2D detector/extractor algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/d3/d61/classcv_1_1KAZE.html";
    return (desc);
  }


  /**
   * Fills the map for converting the type variable.
   */
  void KAZEAlgorithm::setupTypeMap() {
    m_typeMap.left.insert(std::pair<QString,int>("DIFF_PM_G1",       0));
    m_typeMap.left.insert(std::pair<QString,int>("DIFF_PM_G2",       1));
    m_typeMap.left.insert(std::pair<QString,int>("DIFF_WEICKERT",    2));
    m_typeMap.left.insert(std::pair<QString,int>("DIFF_CHARBONNIER", 3));
  }


  /**
   * Creates an instance of the algorithm.
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *KAZEAlgorithm::create(const PvlFlatMap &vars, const QString &config) {
    return ( new KAZEAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool KAZEAlgorithm::hasDetector() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool KAZEAlgorithm::hasExtractor() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool KAZEAlgorithm::hasMatcher() const { 
    return false; 
  }


  /**
   * Returns the variables and their values used by the algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap KAZEAlgorithm::getAlgorithmVariables( ) const {
    KAZEPtr algorithm = m_algorithm.dynamicCast<KAZEType>();
    PvlFlatMap variables;
    variables.add("Diffusivity",   m_typeMap.right.at(algorithm->getDiffusivity()));
    variables.add("Extended",      toString(algorithm->getExtended()));
    variables.add("NOctaveLayers", toString(algorithm->getNOctaveLayers()));
    variables.add("NOctaves",      toString(algorithm->getNOctaves()));
    variables.add("Threshold",     toString(algorithm->getThreshold()));
    variables.add("Upright",       toString(algorithm->getUpright()));
    return (variables);
  }


/**
 * Set parameters as provided by the variables
 * 
 * @param variables Container of parameters to set
 * 
 * @return @b int Number of variables actually set
 * 
 * @throws IException::User "The input value is not valid for KAZE's [Type] variable"
 */
  int KAZEAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {
    KAZEPtr algorithm = m_algorithm.dynamicCast<KAZEType>();

    int numSet(0);
    if ( variables.exists("Extended") ) { 
      algorithm->setExtended(toInt(variables.get("Extended")));
      numSet++;
    }

    if ( variables.exists("Threshold") ) { 
      algorithm->setThreshold(toInt(variables.get("Threshold")));
      numSet++;
    }

    if ( variables.exists("NOctaveLayers") ) { 
      algorithm->setNOctaveLayers(toInt(variables.get("NOctaveLayers")));
      numSet++;
    }

    if ( variables.exists("NOctaves") ) { 
      algorithm->setNOctaves(toInt(variables.get("NOctaves")));
      numSet++;
    }

    if ( variables.exists("Upright") ) { 
      algorithm->setUpright(toInt(variables.get("Upright")));
      numSet++;
    }

    if ( variables.exists("Diffusivity") ) {
      QString value = variables.get("Diffusivity");
      bool isInt;
      // Check if the value is an integer
      int intValue = value.toInt(&isInt);
      try {
        if (isInt) {
          // If it is an integer make sure it is a valid option
          m_typeMap.right.at(intValue);
        }
        else {
          // If it is a string, then convert it to an integer
          intValue = m_typeMap.left.at(value);
        }
      }
      catch (std::exception &e) {
        QString msg = "The input value [" + value +
                      "] is not valid for KAZE's [Type] variable";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      algorithm->setDiffusivity(intValue);
      numSet++;
    }

    return (numSet);
  }

};  // namespace Isis
