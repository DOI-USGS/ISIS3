#ifndef MatcherAlgorithms_h
#define MatcherAlgorithms_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include <QList>

#include "FeatureAlgorithm.h"
#include "PvlFlatMap.h"
#include "PvlObject.h"


namespace Isis {

/**
 * @brief Generic container for ISIS/OpenCV-type feature matcher algorithms
 *
 * This class provides a container for the three elements of feature-based
 * matching: detector, extractor and matcher. An addition container of robust
 * matcher outlier detection is added as a "paramters" specification.
 *
 * @author  2016-11-29 Kris Becker
 *
 * @internal
 *   @history 2016-11-29 Kris Becker - Original Version
 */

class MatcherAlgorithms {
  public:
    MatcherAlgorithms();

    MatcherAlgorithms(FeatureAlgorithmPtr &detector,
                      FeatureAlgorithmPtr &extractor,
                      MatcherAlgorithmPtr &matcher,
                      PvlFlatMap parameters = PvlFlatMap());

    virtual ~MatcherAlgorithms();

    virtual bool isValid() const;
    bool validate(const bool &throwOnErrors = true) const;

    Feature2DAlgorithm &detector() const;
    Feature2DAlgorithm &extractor() const;
    DescriptorMatcherAlgorithm &matcher() const;

    const PvlFlatMap &parameters() const;

    PvlObject info(const QString &name = "MatcherAlgorithms") const;

  private:
    FeatureAlgorithmPtr  m_detector;  //!< Detector algorithm
    FeatureAlgorithmPtr  m_extractor; //!< Extractor algorithm
    MatcherAlgorithmPtr  m_matcher;   //!< Matcher algorithm
    PvlFlatMap m_parameters;                //!< Parameters for matcher

};

};  // namespace Isis
#endif
