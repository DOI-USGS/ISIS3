#ifndef MSERAlgorithm_h
#define MSERAlgorithm_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "opencv2/features2d.hpp"

#include <QVariant>

#include "MSERExtractor.h"
#include "FeatureAlgorithm.h"

namespace Isis {

/**
 * @brief MSER Feature matcher algorithm
 *
 * This class provides the OpenCV3 MSER Feature2D algortithm. Only the
 * necesary methods are implemented here.
 *
 * @author  2016-12-08 Jesse Mapel
 *
 * @internal
 *   @history 2016-12-08 Jesse Mapel - Original Version
 */

class MSERAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    MSERAlgorithm();
    MSERAlgorithm( const PvlFlatMap &cvars, const QString &config = QString() );
    virtual ~MSERAlgorithm();

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
    typedef MSERExtractor     MSERType;
    typedef cv::Ptr<MSERType> MSERPtr;
};

};  // namespace Isis
#endif
