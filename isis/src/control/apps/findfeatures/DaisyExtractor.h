#ifndef DaisyExtractor_h
#define DaisyExtractor_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "opencv2/xfeatures2d.hpp"
#include "opencv2/opencv.hpp"

namespace Isis {

/**
 * @brief Wrap of OpenCV3 DAISY algorithm to implement pure virtual functions.
 *
 * This class wraps the abstract DAISY algorithm so it can be used.
 *
 * @author 2016-12-20 Kristin Berry
 *
 * @internal
 *  @history 2016-12-20 Kristin Berry - Original Version
 *
 */

class DaisyExtractor : public cv::xfeatures2d::DAISY {

public:

    virtual void compute( cv::InputArray image, std::vector<cv::KeyPoint>& keypoints,
                          cv::OutputArray descriptors );

    virtual void compute( cv::InputArray image, cv::Rect roi, cv::OutputArray descriptors );

    virtual void compute( cv::InputArray image, cv::OutputArray descriptors );

    virtual void GetDescriptor( double y, double x, int orientation, float* descriptor ) const;

    virtual bool GetDescriptor( double y, double x, int orientation, float* descriptor,
                                double* H ) const;

    virtual void GetUnnormalizedDescriptor( double y, double x, int orientation,
                                            float* descriptor ) const;

    virtual bool GetUnnormalizedDescriptor( double y, double x, int orientation,
                                            float* descriptor , double *H ) const;
};

}
#endif
