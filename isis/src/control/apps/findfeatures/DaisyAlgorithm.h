#ifndef DaisyAlgorithm_h
#define DaisyAlgorithm_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "DaisyExtractor.h"
#include "FeatureAlgorithm.h"
#include "opencv2/xfeatures2d.hpp"

#include <boost/bimap.hpp>

#include <QString>
#include <QVariant>

namespace Isis {

/**
 * @brief Daisy Feature matcher algorithm
 *
 * This class provides the OpenCV3 SURF Feature2D algortithm. Only the
 * necesary methods are implemented here.
 *
 * @author  2016-11-30 Kristin Berry
 *
 * @internal
 *   @history 2016-11-30 Kristin Berry - Original Version
 */

class DaisyAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    DaisyAlgorithm();
    DaisyAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
#if 0
    DaisyAlgorithm(float radius=15, int q_radius=3, int q_theta=8, int q_hist=8,
                   int norm=DAISY::NRM_NONE, InputArray H=noArray(),
                   bool interpolation=true, bool use_orientation=false);
#endif
    virtual ~DaisyAlgorithm();

    QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm  *create(const PvlFlatMap &vars,
                                      const QString &config = QString());

    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int        setAlgorithmVariables(const PvlFlatMap &variables);

    void       setupTypeMap();
    PvlFlatMap setupParameters();

    boost::bimap<QString, int> m_typeMap; //!< Bi-directional map for converting type values.

  private:
    typedef DaisyExtractor       DAISYType;
    typedef cv::Ptr<DAISYType>   DAISYPtr;

};

};  // namespace Isis
#endif
