#ifndef KAZEAlgorithm_h
#define KAZEAlgorithm_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FeatureAlgorithm.h"
#include "opencv2/features2d.hpp"

#include <QMap>
#include <QString>
#include <QVariant>

#include <boost/bimap.hpp>

namespace Isis {

/**
 * @brief KAZE Feature matcher algorithm
 *
 * This class provides the OpenCV3 KAZE Feature2D algortithm. Only the
 * necesary methods are implemented here.
 *
 * @author  2016-12-09 Kristin Berry
 *
 * @internal
 *   @history 2016-12-09 Kristin Berry - Original Version
 */

class KAZEAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    KAZEAlgorithm();
    KAZEAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
    virtual ~KAZEAlgorithm();

    virtual QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm *create(const PvlFlatMap &vars,
                                     const QString &config = QString());

    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);
    void setupTypeMap();

    boost::bimap<QString, int> m_typeMap; //!< Bi-directional map for converting type values.

  private:
    typedef cv::KAZE                KAZEType;
    typedef cv::Ptr<KAZEType>       KAZEPtr;

};


};  // namespace Isis
#endif
