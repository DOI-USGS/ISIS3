/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <numeric>


#include "IException.h"
#include "ScalingTransform.h"

namespace Isis {

ScalingTransform::ScalingTransform() : ImageTransform("ScaleTransform"),
                                       m_scale(1.0) { }

ScalingTransform::ScalingTransform(const double &scale, const QString &name) :
                                   ImageTransform(name), m_scale(scale) { }


/**
 * @brief Implementation of scaling transform
 *
 * @author 2014-07-02 Kris Becker
 *
 * @param scale   Scale to apply to source image
 * @param source  Input images to apply transform to
 *
 * @return cv::Mat Returns the scaled image
 */
cv::Mat ScalingTransform::render(const cv::Mat &image) const {
  cv::Size dstSize(static_cast<int>(image.cols * m_scale + 0.5),
                   static_cast<int>(image.rows * m_scale + 0.5));
  cv::Mat scaled;
  cv::resize(image, scaled, dstSize, cv::INTER_AREA);
  return (scaled);
}

cv::Point2f ScalingTransform::forward(const cv::Point2f &point) const {
  cv::Point2f spoint;
  spoint.x = point.x * m_scale;
  spoint.y = point.y * m_scale;
  return (spoint);
}

cv::Point2f ScalingTransform::inverse(const cv::Point2f &point) const {
  cv::Point2f spoint;
  spoint.x = point.x / m_scale;
  spoint.y = point.y / m_scale;
  return (spoint);
}

}  // namespace Isis
