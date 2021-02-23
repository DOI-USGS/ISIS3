#ifndef GFTTAlgorithm_h
#define GFTTAlgorithm_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "FeatureAlgorithm.h"
#include "opencv2/features2d.hpp"

#include <QVariant>

namespace Isis {

/**
 * @brief GFTT Feature matcher algorithm
 *
 * This class provides the OpenCV3 GFTT Feature2D algortithm. Only the
 * necesary methods are implemented here.
 *
 * @author  2016-12-07 Jesse Mapel
 *
 * @internal
 *   @history 2016-12-07 Jesse Mapel - Original Version
 */

class GFTTAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    GFTTAlgorithm();
    GFTTAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
    virtual ~GFTTAlgorithm();

    QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm *create(const PvlFlatMap &vars,
                                     const QString &config = QString());
    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);

  private:
    typedef cv::GFTTDetector  GFTTType;
    typedef cv::Ptr<GFTTType> GFTTPtr;
};

};  // namespace Isis
#endif
