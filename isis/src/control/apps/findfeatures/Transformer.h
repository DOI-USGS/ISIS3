#ifndef Transformer_h
#define Transformer_h

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

namespace Isis {

/**
 * @brief Provide a series of image transforms
 *
 * This class serves as a container class for all image transformations and
 * point conversions, both forward and inverse.
 *
 * As each image is read, the list of transforms are applied for each transform
 * added to the this class. There is typically a list of transforms for each
 * image.
 *
 * @author 2015-10-03 Kris Becker
 * @internal
 *   @history 2015-10-03 Kris Becker - Original Version
 *   @history 2016-04-05 Kris Becker Completed documentation
 */

  class Transformer {
    public:
      typedef ImageTransformQList::iterator       ImageTransformIterator;
      typedef ImageTransformQList::const_iterator ImageTransformConstIterator;

      Transformer();
      virtual ~Transformer();

      int size() const;

      void add(ImageTransform *transform);
      cv::Mat render(const cv::Mat &image) const;

      cv::Point2f forward(const cv::Point2f &point) const;
      cv::Point2f inverse(const cv::Point2f &point) const;

      ImageTransformConstIterator begin() const;
      ImageTransformConstIterator end() const;

      void clear();

    private:
      ImageTransformQList m_transforms;  //!<  List of transforms
  };

}  // namespace Isis
#endif
