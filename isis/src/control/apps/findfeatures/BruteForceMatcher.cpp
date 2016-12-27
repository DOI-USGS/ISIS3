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

#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "PvlFlatMap.h"

#include <QVariant>

#include "BruteForceMatcher.h"

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  BruteForceMatcher::BruteForceMatcher() : 
        DescriptorMatcherAlgorithm("BFMatcher", "DecriptorMatcher",
                                   cv::makePtr<cv::BFMatcher>()) {
    m_normTypeMap = setupNormTypeMap();
    PvlFlatMap variables;
    variables.add("NormType",   "NORM_L2");
    variables.add("CrossCheck", "false");
    m_variables.merge(variables);
  }


  /**
   * Constructs the algorithm with the input variables
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   * @param normType The type of norm to use.  Options are
   *                 2: The L1 norm,
   *                 4: The L2 norm (DEFAULT),
   *                 6: The Hamming norm, or
   *                 7: The Hamming 2 norm.
   * @param crossCheck If true; when j, the nearest neightbor to a point i, is
   *                   found it will be checked that i is the nearest neighbor
   *                   to j.  Defaults to false.
   * 
   * @throws IException::User "The input value is not valid for
   *                           BruteForceMatcher's [NormType] variable"
   * 
   * @see cv::NormTypes
   */
  BruteForceMatcher::BruteForceMatcher(const PvlFlatMap &cvars, const QString &config,
                                       const int normType, const bool crossCheck) :
        DescriptorMatcherAlgorithm("BFMatcher", "DecriptorMatcher",
                                   cv::makePtr<cv::BFMatcher>(normType, crossCheck), cvars) {
    m_normTypeMap = setupNormTypeMap();
    setConfig(config);
    PvlFlatMap variables;
    try {
      variables.add("NormType", m_normTypeMap.right.at(normType));
    }
    catch (std::exception &e){
      QString msg = "The input value [" + toString(normType) +
                    "] is not valid for BruteForceMatcher's [NormType] variable";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    variables.add("CrossCheck", toString(crossCheck));
    m_variables.merge(variables);
  }


  /**
   * Destroys the algorithm
   */
  BruteForceMatcher::~BruteForceMatcher() { }


  /**
   * Sets up and returns the bi-directional map for norm type.
   * 
   * @return @b boost:bimap<QString,int> The map between name and int for norm type.
   *
   * @see cv::NormTypes
   */
  boost::bimap<QString, int> BruteForceMatcher::setupNormTypeMap() {
    boost::bimap<QString, int> normTypeMap;

    normTypeMap.left.insert(std::pair<QString,int>("NORM_L1",       2));
    normTypeMap.left.insert(std::pair<QString,int>("NORM_L2",       4));
    normTypeMap.left.insert(std::pair<QString,int>("NORM_HAMMING",  6));
    normTypeMap.left.insert(std::pair<QString,int>("NORM_HAMMING2", 7));

    return ( normTypeMap );
  }


  /**
   * Returns a description of the algorithm.
   * 
   * @return @b QString A description of the algorithm.
   */
  QString BruteForceMatcher::description() const {
    QString desc = "The OpenCV BFMatcher DescriptorMatcher matcher algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/d3/da1/classcv_1_1BFMatcher.html";
    return (desc);
  }


  /**
   * Creates an instance of the algorithm.
   * 
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   * 
   * @throws IException::User "The input value is not valid for
   *                           BruteForceMatcher's [NormType] variable"
   */
  DescriptorMatcherAlgorithm *BruteForceMatcher::create(const PvlFlatMap &vars,
                                                        const QString &config) {
    boost::bimap<QString, int> normTypeMap = setupNormTypeMap();
    const QString norm = vars.get("NormType", "NORM_L2");
    bool isInt;
    int normType = norm.toInt(&isInt);
    try {
      if (isInt) {
        normTypeMap.right.at(normType);
      }
      else {
        normType = normTypeMap.left.at(norm);
      }
    }
    catch (std::exception &e) {
        QString msg = "The input value [" + norm +
                      "] is not valid for BruteForceMatcher's [NormType] variable";
        throw IException(IException::User, msg, _FILEINFO_);
    }

    const bool crossCheck = toBool(vars.get("CrossCheck", "false"));

    return ( new BruteForceMatcher(vars, config, normType, crossCheck) );
  }


  /**
   * Returns true if the algorithm has a detector. 
   *  
   * @return @b true if the algorithm has a detector. 
   */
  bool BruteForceMatcher::hasDetector() const { 
    return false; 
  }


  /**
   * Returns true if the algorithm has an extractor. 
   *  
   * @return @b true if the algorithm has an extractor. 
   */
  bool BruteForceMatcher::hasExtractor() const { 
    return false; 
  }


  /**
   * Returns true if the algorithm has a matcher. 
   *  
   * @return @b true if the algorithm has a matcher. 
   */
  bool BruteForceMatcher::hasMatcher() const { 
    return true; 
  }


  /**
   * Returns the variables and their values used by the algorithm.
   * 
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap BruteForceMatcher::getAlgorithmVariables( ) const {
    return ( variables() );
  }


/**
 * @brief Set parameters as provided by the variables
 * 
 * @param variables Container of parameters to set
 * 
 * @return @b int Always -1, variables cannot be set after initialization.
 * 
 * @throws IException::Programmer "BruteForceMatcher does not have the ability
 *                                 to set algorithm parameters."
 */
  int BruteForceMatcher::setAlgorithmVariables(const PvlFlatMap &variables) {
    QString msg = "BruteForceMatcher does not have the ability to set algorithm parameters.";
    throw IException(IException::Programmer, msg, _FILEINFO_);

    return (-1);
  }

};  // namespace Isis
