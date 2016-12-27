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

#include <boost/bimap.hpp>
#include <boost/foreach.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "ORBAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the ORB algorithm with default variables.
   */
  ORBAlgorithm::ORBAlgorithm() : 
                 Feature2DAlgorithm("ORB", "Feature2D",
                                     ORBType::create()) {  
    setupTypeMap();
    m_variables.merge( getAlgorithmVariables() );
  }


  /**
   * Constructs the ORB algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  ORBAlgorithm::ORBAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                  Feature2DAlgorithm("ORB", "Feature2D", 
                                      ORBType::create(), cvars) {
    setupTypeMap();
    setConfig(config);
    setAlgorithmVariables(cvars);
    m_variables.merge(getAlgorithmVariables());
  }


  /**
   * Destroys the ORB algorithm
   */
  ORBAlgorithm::~ORBAlgorithm() { }


  /**
   * Returns a description of the ORB algorithm.
   * 
   * @return @b QString A description of the ORB algorithm.
   */
  QString ORBAlgorithm::description() const {
    QString desc = "The OpenCV ORB Feature2D detector/extractor algorithm."
                   " See the documentation at "
      "http://docs.opencv.org/3.1.0/db/d95/classcv_1_1ORB.html";
    return (desc);
  }


  /**
   * Fills the map for converting the type variable.
   */
  void ORBAlgorithm::setupTypeMap() {
    m_typeMap.left.insert(std::pair<QString,int>("kBytes",      32));
    m_typeMap.left.insert(std::pair<QString,int>("HARRIS_SCORE", 0));
    m_typeMap.left.insert(std::pair<QString,int>("FAST_SCORE",   1));
  }


  /**
   * Creates an instance of the ORB algorithm.
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *ORBAlgorithm::create(const PvlFlatMap &vars, const QString &config) {
    return ( new ORBAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool ORBAlgorithm::hasDetector() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool ORBAlgorithm::hasExtractor() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool ORBAlgorithm::hasMatcher() const { 
    return false; 
  }


  /**
   * Returns the variables and their values used by the ORB algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap ORBAlgorithm::getAlgorithmVariables( ) const {
    ORBPtr algorithm = m_algorithm.dynamicCast<ORBType>();

    PvlFlatMap variables;
    variables.add("nfeatures",     toString(algorithm->getMaxFeatures()));
    variables.add("scaleFactor",   toString(algorithm->getScaleFactor()));
    variables.add("nlevels",       toString(algorithm->getNLevels()));
    variables.add("edgeThreshold", toString(algorithm->getEdgeThreshold()));
    variables.add("firstLevel",    toString(algorithm->getFirstLevel()));
    variables.add("WTA_K",         toString(algorithm->getWTA_K()));
    variables.add("scoreType",     m_typeMap.right.at(algorithm->getScoreType()));
    variables.add("patchSize",     toString(algorithm->getPatchSize()));
    variables.add("fastThreshold", toString(algorithm->getFastThreshold()));
    return (variables);
  }


/**
 * Set parameters as provided by the variables
 * 
 * @param variables Container of parameters to set
 * 
 * @return @b int Number of variables actually set
 * 
 * @throws IException::User "The input value is not valid for ORB's [scoreType] variable"
 */
  int ORBAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {

    ORBPtr algorithm = m_algorithm.dynamicCast<ORBType>();

    int numSet(0);
    if ( variables.exists("nfeatures") ) { 
      algorithm->setMaxFeatures(toInt(variables.get("nfeatures")));
      numSet++;
    }

    if ( variables.exists("scaleFactor") ) { 
      algorithm->setScaleFactor(toDouble(variables.get("scaleFactor")));
      numSet++;
    }


     if ( variables.exists("nlevels") ) { 
      algorithm->setNLevels(toInt(variables.get("nlevels")));
      numSet++;
     }

     if ( variables.exists("edgeThreshold") ) { 
      algorithm->setEdgeThreshold(toInt(variables.get("edgeThreshold")));
      numSet++;
     }     

     if ( variables.exists("firstLevel") ) { 
      algorithm->setFirstLevel(toInt(variables.get("firstLevel")));
      numSet++;
     }     

     if ( variables.exists("WTA_K") ) { 
      algorithm->setWTA_K(toInt(variables.get("WTA_K")));
      numSet++;
     }    


     if ( variables.exists("scoreType") ) {
       QString value = variables.get("scoreType");
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
                       "] is not valid for ORB's [Type] variable";
         throw IException(IException::User, msg, _FILEINFO_);
       }

      algorithm->setScoreType(intValue);
      numSet++;

     if ( variables.exists("patchSize") ) { 
      algorithm->setPatchSize(toInt(variables.get("patchSize")));
      numSet++;
     }     

     if ( variables.exists("fastThreshold") ) { 
      algorithm->setPatchSize(toInt(variables.get("fastThreshold")));
      numSet++;
     }
   }
   return (numSet);
  }

};  // namespace Isis

