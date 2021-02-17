#ifndef CropTransform_h
#define CropTransform_h

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
#include "ImageTransform.h"
#include "GenericTransform.h"

namespace Isis {

/**
 * @brief Crop a section from an image using a translation (affine) transform
 *
 * This image transform will crop a section of an image stored in an OpenCV
 * matrix and return the cropped version of the image.
 *
 * @author 2015-10-01 Kris Becker
 * @internal
 *   @history 2015-10-01 Kris Becker - Original Version
 */

class CropTransform : public GenericTransform {
  public:
    typedef GenericTransform::RectArea   RectArea;
    CropTransform();
    CropTransform(const QString &name);
    CropTransform(const QString &name, const cv::Size &orgSize,
                  const cv::Size &tfSize, const cv::Mat &tform);
    CropTransform(const QString &name, const RectArea &region);
    virtual ~CropTransform();

    virtual cv::Mat render(const cv::Mat &image) const;

  protected:
    cv::Rect findCrop(const cv::Mat &tform, const cv::Size &imSize,
                      const cv::Size &tfSize) const;

  private:
    RectArea    m_crop;
};

}  // namespace Isis
#endif
