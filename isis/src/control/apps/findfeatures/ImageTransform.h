#ifndef ImageTransform_h
#define ImageTransform_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

#include "FeatureMatcherTypes.h"

namespace Isis {

/**
 * @brief OpenCV-based image transformation base class
 *
 *
 * @author 2014-07-01 Kris Becker
 * @internal
 *   @history 2014-07-01 Kris Becker - Original Version
 *   @history 2017-06-22 Jesse Mapel - Added a warning to render about
 *                                     cv::Mat copying and modification.
 *                                     References #4904.
 */

class ImageTransform {
  public:
    typedef cv::Rect  RectArea;

    ImageTransform();
    ImageTransform(const QString &name);

    virtual ~ImageTransform();

    QString name() const;

    virtual cv::Mat render(const cv::Mat &image) const;
    virtual cv::Point2f forward(const cv::Point2f &point) const;
    virtual cv::Point2f inverse(const cv::Point2f &point) const;


    static RectArea boundingBox(const cv::Mat &tform,
                                const RectArea &region,
                                const cv::Size &bounds);
    static RectArea transformedSize(const cv::Mat &tmat,
                                    const cv::Size &imSize,
                                    cv::Mat &tmat_t);

    static std::vector<cv::Point2f> corners(const cv::Size &imSize);
    static std::vector<cv::Point2f> corners(const RectArea &region);


    static cv::Mat translation(const double xoffset, const double yoffset);

  private:
    QString          m_name;
};

///!<   Shared ImageTransformpointer that everyone can use
typedef QSharedPointer<ImageTransform> SharedImageTransform;

///!<  Define a ImageTransform list
typedef QList<SharedImageTransform> ImageTransformQList;

}  // namespace Isis
#endif
