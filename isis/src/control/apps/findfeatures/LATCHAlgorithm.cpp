/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <boost/foreach.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "LATCHAlgorithm.h"
#include <QVariant>

namespace Isis {


  /**
   * Constructs the algorithm with default variables.
   */
  LATCHAlgorithm::LATCHAlgorithm() :
                 Feature2DAlgorithm("LATCH", "Feature2D",
                                    LATCHType::create()) {

    setupParameters();
  }


  /**
   * Constructs the algorithm with the input variables
   *
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  LATCHAlgorithm::LATCHAlgorithm(const PvlFlatMap &cvars, const QString &config) :
                  Feature2DAlgorithm("LATCH", "Feature2D",
                                     LATCHType::create(), cvars) {
    setConfig(config);
    PvlFlatMap variables = setupParameters();
    variables.merge(cvars);
    const int bytes = toInt(variables.get("Bytes"));
    const bool rotationInvariance = toBool(variables.get("RotationInvariance"));
    const int halfSSDSize = toInt(variables.get("HalfSSDSize"));

    m_algorithm = LATCHType::create(bytes, rotationInvariance, halfSSDSize);

    m_variables.merge(variables);
  }


  /**
   * Destroys the algorithm
   */
  LATCHAlgorithm::~LATCHAlgorithm() { }


  /**
   * Sets up the algorithm parameters with default values.
   *
   * @return PvlFlatMap Algorithm parameters and their default values.
   */
  PvlFlatMap LATCHAlgorithm::setupParameters() {
    PvlFlatMap variables;
    variables.add("Bytes",              "32");
    variables.add("RotationInvariance", "true");
    variables.add("HalfSSDSize",        "3");
    m_variables.merge(variables);
    return (m_variables);
  }


  /**
   * Returns a description of the algorithm.
   *
   * @return @b QString A description of the algorithm.
   */
  QString LATCHAlgorithm::description() const {
    QString desc = "The OpenCV LATCH Feature2D detector/extractor algorithm."
                   " See the documentation at "
     "http://docs.opencv.org/3.1.0/d6/d36/classcv_1_1xfeatures2d_1_1LATCH.html";
    return (desc);
  }


  /**
   * Creates an instance of the algorithm.
   *
   * @param cvars  The variables and values the algorithm will use.
   *               Variables that are not included will be set to their default.
   * @param config The config string used to construct cvars.
   */
  Feature2DAlgorithm *LATCHAlgorithm::create(const PvlFlatMap &vars, const QString &config) {
    return ( new LATCHAlgorithm(vars, config) );
  }


  /**
   * Returns true if the algorithm has a detector.
   *
   * @return @b true if the algorithm has a detector.
   */
  bool LATCHAlgorithm::hasDetector() const {
    return false;
  }


  /**
   * Returns true if the algorithm has an extractor.
   *
   * @return @b true if the algorithm has an extractor.
   */
  bool LATCHAlgorithm::hasExtractor() const {
    return true;
  }


  /**
   * Returns true if the algorithm has a matcher.
   *
   * @return @b true if the algorithm has a matcher.
   */
  bool LATCHAlgorithm::hasMatcher() const {
    return false;
  }


  /**
   * Returns the variables and their values used by the algorithm.
   *
   * @return @b PvlFlatMap The variables and their values as keyword, value pairs.
   */
  PvlFlatMap LATCHAlgorithm::getAlgorithmVariables( ) const {
    return (variables());
  }


/**
 * @brief Set parameters as provided by the variables
 *
 * @param variables Container of parameters to set
 *
 * @return @b int Always -1, variables cannot be set after initialization.
 *
 * @throws IException::Programmer "LATCHAlgorithm does not have the ability
 *                                 to set algorithm parameters."
 */
  int LATCHAlgorithm::setAlgorithmVariables(const PvlFlatMap &variables) {
    QString msg = "LATCHAlgorithm does not have the ability to set algorithm parameters.";
    throw IException(IException::Programmer, msg, _FILEINFO_);

    return (-1);
  }

};  // namespace Isis
