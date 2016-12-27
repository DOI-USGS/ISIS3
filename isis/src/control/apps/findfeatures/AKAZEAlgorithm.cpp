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

#include "AKAZEAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  AKAZEAlgorithm::AKAZEAlgorithm() : 
                  Feature2DAlgorithm("AKAZE", "Feature2D",
                                     AKAZEType::create()) {  
    setupMaps();
    m_variables.merge(getAlgorithmVariables());
  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  AKAZEAlgorithm::AKAZEAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                  Feature2DAlgorithm("AKAZE", "Feature2D", 
                                     AKAZEType::create(), cvars) {
    setupMaps();
    setConfig(config);
    setAlgorithmVariables(cvars);
    m_variables.merge(getAlgorithmVariables());
  }


  /**
   * Destroys the algorithm
   */
  AKAZEAlgorithm::~AKAZEAlgorithm() { }


  /**
   * Fills the maps for converting the DescriptorType & Diffusivity variable.
   */
  void AKAZEAlgorithm::setupMaps() {
    // DescriptorType map. Int values are from OpenCV's API.
    m_descriptorTypeMap.left.insert(std::pair<QString,int>("DESCRIPTOR_KAZE_UPRIGHT", 2));
    m_descriptorTypeMap.left.insert(std::pair<QString,int>("DESCRIPTOR_KAZE",         3));
    m_descriptorTypeMap.left.insert(std::pair<QString,int>("DESCRIPTOR_MLDB_UPRIGHT", 4));
    m_descriptorTypeMap.left.insert(std::pair<QString,int>("DESCRIPTOR_MLDB",         5));

    // Diffusivity map. Int values are from OpenCV's API.
    m_diffusivityMap.left.insert(std::pair<QString,int>("DIFF_PM_G1",       0));
    m_diffusivityMap.left.insert(std::pair<QString,int>("DIFF_PM_G2",       1));
    m_diffusivityMap.left.insert(std::pair<QString,int>("DIFF_WEICKERT",    2));
    m_diffusivityMap.left.insert(std::pair<QString,int>("DIFF_CHARBONNIER", 3));
  }


  /**
   * Returns a description of the algorithm.
   * 
   * @return @b QString A description of the algorithm.
   */
  QString AKAZEAlgorithm::description() const {
    QString desc = "The OpenCV AKAZE Feature2D detector/extractor algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/d8/d30/classcv_1_1AKAZE.html";
    return (desc);
  }


  /**
   * Creates an instance of the algorithm.
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *AKAZEAlgorithm::create(const PvlFlatMap &vars, 
                                                            const QString &config) {
    return ( new AKAZEAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool AKAZEAlgorithm::hasDetector() const { 
    return false; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool AKAZEAlgorithm::hasExtractor() const { 
    return true; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool AKAZEAlgorithm::hasMatcher() const { 
    return false; 
  }


  /**
   * Returns the variables and their values used by the algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap AKAZEAlgorithm::getAlgorithmVariables( ) const {
    AKAZEPtr algorithm = m_algorithm.dynamicCast<AKAZEType>();
    PvlFlatMap variables;
    variables.add("DescriptorType",     m_descriptorTypeMap.right.at(
                                              algorithm->getDescriptorType()));
    variables.add("DescriptorSize",     toString(algorithm->getDescriptorSize()));
    variables.add("DescriptorChannels", toString(algorithm->getDescriptorChannels()));
    variables.add("Threshold",          toString(algorithm->getThreshold()));
    variables.add("NOctaves",           toString(algorithm->getNOctaves()));
    variables.add("NOctaveLayers",      toString(algorithm->getNOctaveLayers()));
    variables.add("Diffusivity",        m_diffusivityMap.right.at(
                                              algorithm->getDiffusivity()));
    return (variables);
  }


/**
 * Set parameters as provided by the variables
 * 
 * @param variables Container of parameters to set
 * 
 * @return @b int Number of variables actually set
 * 
 * @throws IException::User "The input value is not valid for AKAZE's [DescriptorType] variable"
 * @throws IException::User "The input value is not valid for AKAZE's [Diffusivity] variable"
 */
  int AKAZEAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {

    AKAZEPtr algorithm = m_algorithm.dynamicCast<AKAZEType>();

    int numSet(0);
    if ( variables.exists("DescriptorType") ) { 
      QString value = variables.get("DescriptorType");
      bool isInt;
      // Check if the value is an integer
      int intValue = value.toInt(&isInt);
      try {
        if (isInt) {
          // If it is an integer make sure it is a valid option
          m_descriptorTypeMap.right.at(intValue);
        }
        else {
          // If it is a string, then convert it to an integer
          intValue = m_descriptorTypeMap.left.at(value);
        }
      }
      catch (std::exception &e) {
        QString msg = "The input value [" + value +
                      "] is not valid for AKAZE's [DescriptorType] variable";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      algorithm->setDescriptorType(intValue);
      numSet++;
    }

    if ( variables.exists("DescriptorSize") ) { 
      algorithm->setDescriptorSize(toInt(variables.get("DescriptorSize")));
      numSet++;
    }

    if ( variables.exists("DescriptorChannels") ) { 
      algorithm->setDescriptorChannels(toInt(variables.get("DescriptorChannels")));
      numSet++;
    }

    if ( variables.exists("Threshold") ) { 
      algorithm->setThreshold(toInt(variables.get("Threshold")));
      numSet++;
    }

    if ( variables.exists("NOctaves") ) { 
      algorithm->setNOctaves(toInt(variables.get("NOctaves")));
      numSet++;
    }

    if ( variables.exists("NOctaveLayers") ) { 
      algorithm->setNOctaveLayers(toInt(variables.get("NOctaveLayers")));
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
          m_diffusivityMap.right.at(intValue);
        }
        else {
          // If it is a string, then convert it to an integer
          intValue = m_diffusivityMap.left.at(value);
        }
      }
      catch (std::exception &e) {
        QString msg = "The input value [" + value +
                      "] is not valid for AKAZE's [Diffusivity] variable";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      algorithm->setDiffusivity(intValue);
      numSet++;
    }

    return (numSet);
  }

};  // namespace Isis
