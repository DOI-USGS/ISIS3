/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include <QList>
#include <QStringList>
#include <boost/foreach.hpp>

#include "Camera.h"
#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "Transformer.h"

namespace Isis {

/* Basic constructor */
Transformer::Transformer() : m_transforms() { }


Transformer::~Transformer() { }

/* Return number of transforms stored in the list */
int Transformer::size() const {
  return ( m_transforms.size() );
}

/**
 * @brief Add a transform to the list
 *
 * Caller supplies a pointer to an image transform and inherits ownership of the
 * pointer. The pointers are stored in a shared pointer to memory management is
 * handled automatically.
 *
 * @param transform Pointer to add to list
 */
void Transformer::add(ImageTransform *transform) {
  m_transforms.append(SharedImageTransform(transform));
  return;
}

/**
 * @brief Apply image transforms to an image
 *
 * The incoming image is rendered in each transform, apply the results of the
 * previous transform to the next transform.
 *
 * @param image  Image to transform
 * @return cv::Mat Final result of all image transforms
 */
cv::Mat Transformer::render(const cv::Mat &image) const {
  cv::Mat t_image = image;
  BOOST_FOREACH ( SharedImageTransform t, m_transforms ) {
    t_image = t->render(t_image);
  }
  return (t_image);
}

/**
 * @brief Convert a point coordinate applied in each transform
 *
 * The point passed into this method has the forward point transform of each
 * ImageTransform applied to result in the final coordinate.
 *
 * @param point        Point to transform with the transform list
 * @return cv::Point2f Transformed point coordinate
 */
cv::Point2f Transformer::forward(const cv::Point2f &point) const {
  cv::Point2f tpoint = point;
  BOOST_FOREACH ( SharedImageTransform t, m_transforms ) {
    tpoint = t->forward(tpoint);
  }
  return (tpoint);
}

/**
 * @brief Convert the inverse of the point from the list of imaage tranforms
 *
 * The given point is assumed to have been appled with the forward() method,
 * and modified or computed in the final transform data space. This method
 * converts the coordinates back the the original coordinate system (typically
 * raw image space) by traversing the list in reverse and applying the inverse
 * point conversion method.
 *
 * @param point        Point to invert
 * @return cv::Point2f Resulting inverted point
 */
cv::Point2f Transformer::inverse(const cv::Point2f &point) const {
  cv::Point2f tpoint = point;
  BOOST_REVERSE_FOREACH ( SharedImageTransform t, m_transforms ) {
    tpoint = t->inverse(tpoint);
  }
  return (tpoint);
}

/* Return the start of the image transform list */
Transformer::ImageTransformConstIterator Transformer::begin() const {
  return ( m_transforms.begin() );
}

/* Return the end of the image transform list */
Transformer::ImageTransformConstIterator Transformer::end() const{
  return ( m_transforms.end() );
}

/* Clear the image transform list automatically freeing memory with shared pointer */
void Transformer::clear() {
  m_transforms.clear();
}


}// namespace Isis
