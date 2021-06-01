#ifndef LUCIDAlgorithm_h
#define LUCIDAlgorithm_h
/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FeatureAlgorithm.h"
#include <QVariant>

namespace Isis {

/**
 * @brief LUCID Feature matcher algorithm
 *
 * This class provides the OpenCV3 LUCID Feature2D algortithm. Only the
 * necesary methods are implemented here.
 *
 * @author  2016-12-12 Jesse Mapel
 *
 * @internal
 *   @history 2016-12-12 Jesse Mapel - Original Version
 */

class LUCIDAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    LUCIDAlgorithm();
    LUCIDAlgorithm(const PvlFlatMap &cvars, const QString &config = QString() );
    virtual ~LUCIDAlgorithm();

    QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm *create( const PvlFlatMap &vars,
                                       const QString &config = QString() );

    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap setupParameters();
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);

  private:
    typedef cv::xfeatures2d::LUCID  LUCIDType;
    typedef cv::Ptr<LUCIDType>      LUCIDPtr;
};

};  // namespace Isis
#endif
