#ifndef ScalingTransform_h
#define ScalingTransform_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>
#include <boost/foreach.hpp>

#include "ImageTransform.h"


namespace Isis {

/**
 * @brief Scale an image size up or down
 *
 *
 * @author 2014-07-01 Kris Becker
 * @internal
 *   @history 2014-07-01 Kris Becker - Original Version
 */

  class ScalingTransform : public ImageTransform {
    public:
      ScalingTransform();
      ScalingTransform(const double &scale,
                       const QString &name = "ScaleTransform");
      virtual ~ScalingTransform() { }

      virtual cv::Mat render(const cv::Mat &image) const;
      virtual cv::Point2f forward(const cv::Point2f &point) const;
      virtual cv::Point2f inverse(const cv::Point2f &point) const;

    private:
      double m_scale;

  };

}  // namespace Isis
#endif
