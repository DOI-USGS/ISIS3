#ifndef AgastAlgorithm_h
#define AgastAlgorithm_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "FeatureAlgorithm.h"

#include <boost/bimap.hpp>

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"

namespace Isis {

/**
 * @brief AGAST Feature matcher algorithm
 *
 * This class provides the OpenCV3 AGAST Feature2D algortithm. Only the
 * necesary methods are implemented here.
 *
 * @author  2016-12-07 Jesse Mapel
 *
 * @internal
 *   @history 2016-12-07 Jesse Mapel - Original Version
 */

class AgastAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    AgastAlgorithm();
    AgastAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
    virtual ~AgastAlgorithm();

    QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm *create(const PvlFlatMap &vars,
                                     const QString &config = QString());

    // Provide information about the abilities of the algorithm
    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);
    void setupTypeMap();

    boost::bimap<QString, int> m_typeMap; //!< Bi-directional map for converting type values.

  private:
    typedef cv::AgastFeatureDetector AgastType;
    typedef cv::Ptr<AgastType>       AgastPtr;
};

};  // namespace Isis
#endif
