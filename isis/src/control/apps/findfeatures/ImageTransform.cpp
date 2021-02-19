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

#include "IException.h"
#include "ImageTransform.h"
#include "IString.h"

namespace Isis {

ImageTransform::ImageTransform() : m_name("ImageTransform") { }

ImageTransform::ImageTransform(const QString &name) : m_name(name) { }

ImageTransform::~ImageTransform() { }

QString ImageTransform::name() const {
  return ( m_name );
}


/**
 * Perform the transformation on an image matrix.
 * Child classes should override this method with to implement transformation
 * process.
 *
 * @param image The input image data matrix to transform.
 *
 * @return @b cv::Mat The transformed matrix.
 *
 * @note The cv::Mat class implicilty shares, so if the input image matrix, or
 *       a copy of it, will be modified you need to clone() the matrix prior
 *       to modification or the input matrix will be changed.
 */
cv::Mat ImageTransform::render(const cv::Mat &image) const {
 return (image);
}

cv::Point2f ImageTransform::forward(const cv::Point2f &point) const {
  return (point);
}
cv::Point2f ImageTransform::inverse(const cv::Point2f &point) const {
  return (point);
}


ImageTransform::RectArea ImageTransform::boundingBox(const cv::Mat &tform,
                                       const ImageTransform::RectArea &region,
                                       const cv::Size &bounds) {

  std::vector<cv::Point2f> b_corners = corners(region);
  std::vector<cv::Point2f> t_corners;
  cv::perspectiveTransform(b_corners, t_corners, tform);

  float xmin(t_corners[0].x), xmax(t_corners[0].x);
  float ymin(t_corners[0].y), ymax(t_corners[0].y);
  for (unsigned int i = 0 ; i < t_corners.size() ; i++) {
#if 0
    std::cout << "(x,y) -> (x',y') = ("
              << b_corners[i].x << "," << b_corners[i].y << ") -> ("
              << t_corners[i].x << "," << t_corners[i].y << ")\n";
#endif
   if ( t_corners[i].x < xmin) xmin = t_corners[i].x;
    if ( t_corners[i].x > xmax) xmax = t_corners[i].x;
    if ( t_corners[i].y < ymin) ymin = t_corners[i].y;
    if ( t_corners[i].y > ymax) ymax = t_corners[i].y;
  }

  cv::Point2f xymin( qMax(0.0f, xmin), qMax(0.0f, ymin) );
  cv::Point2f xymax( qMin(xmax, bounds.width-1.0f),
                     qMin(ymax, bounds.height-1.0f) );
  cv::Rect bbox( xymin.x , xymin.y,
                 (int) (xymax.x-xymin.x+0.5),
                 (int) (xymax.y-xymin.y+0.5) );
  return ( bbox );
}

ImageTransform::RectArea ImageTransform::transformedSize(const cv::Mat &tmat,
                                                        const cv::Size &imSize,
                                                        cv::Mat &tmat_t) {

  std::vector<cv::Point2f> t_corners;
  cv::perspectiveTransform(corners(imSize), t_corners, tmat);

  double xmin(t_corners[0].x), xmax(t_corners[0].x);
  double ymin(t_corners[0].y), ymax(t_corners[0].y);
  for (unsigned int i = 1 ; i < t_corners.size() ; i++) {
    if ( t_corners[i].x < xmin) xmin = t_corners[i].x;
    if ( t_corners[i].x > xmax) xmax = t_corners[i].x;
    if ( t_corners[i].y < ymin) ymin = t_corners[i].y;
    if ( t_corners[i].y > ymax) ymax = t_corners[i].y;
  }

#if 0
  std::cout << "\nImageSize:    " << imSize.width << ", " << imSize.height << "\n";
  std::cout << "Min x,y:      " << xmin << ", " << ymin << "\n";
  std::cout << "NewImageSize: " << (xmax-xmin) << ", " << (ymax-ymin) << "\n";
#endif

  // Compute translation to orient the image at line 0.
  cv::Mat zMap = translation(-xmin, -ymin);
  tmat_t = zMap * tmat;
  return ( RectArea(xmin, ymin, (int) (xmax-xmin+0.5), (int) (ymax-ymin+0.5)) );
}

std::vector<cv::Point2f> ImageTransform::corners(const cv::Size &imSize) {
  std::vector<cv::Point2f> points;
  points.push_back(cv::Point2f(0.0, 0.0));
  points.push_back(cv::Point2f(imSize.width - 1.0, 0.0));
  points.push_back(cv::Point2f(imSize.width - 1.0, imSize.height - 1.0));
  points.push_back(cv::Point2f(0.0, imSize.height - 1.0));
  return ( points );
}

std::vector<cv::Point2f> ImageTransform::corners(const ImageTransform::RectArea &region) {
  std::vector<cv::Point2f> points;
  points.push_back(cv::Point2f(region.x, region.y) );
  points.push_back(cv::Point2f(region.x + region.width - 1.0, region.y));
  points.push_back(cv::Point2f(region.x + region.width - 1.0,
                               region.y + region.height - 1.0));
  points.push_back(cv::Point2f(region.x, region.y + region.height - 1.0));
  return ( points );
}

cv::Mat ImageTransform::translation(const double xoffset, const double yoffset) {
  cv::Mat tmat = (cv::Mat_<double>(3,3) << 1.0, 0.0, xoffset,
                                           0.0, 1.0, yoffset,
                                           0.0, 0.0, 1.0);
  return (tmat);
}


} // namespace Isis
