#ifndef BruteForceMatcher_h
#define BruteForceMatcher_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <boost/bimap.hpp>
#include "FeatureAlgorithm.h"
#include <QVariant>

namespace Isis {

/**
 * @brief Brute Force Feature matcher algorithm
 *
 * This class provides the OpenCV3 BFMatcher DescriptorMatcher algortithm. Only the
 * necesary methods are implemented here.
 *
 * @author  2016-12-08 Jesse Mapel
 *
 * @internal
 *   @history 2016-12-08 Jesse Mapel - Original Version
 */

class BruteForceMatcher : public DescriptorMatcherAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    BruteForceMatcher();
    BruteForceMatcher(const PvlFlatMap &cvars, const QString &config = QString(),
                      const int normType = 4, const bool crossCheck = false);
    virtual ~BruteForceMatcher();

    QString description() const;

    // Required for all algorithms
    static DescriptorMatcherAlgorithm *create(const PvlFlatMap &vars,
                                              const QString &config = QString());

    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);
    static boost::bimap<QString, int> setupNormTypeMap();

    boost::bimap<QString, int> m_normTypeMap; /**!< Bi-directional map for converting
                                                   NormType values.*/
};

};  // namespace Isis
#endif
