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

#include <QtGlobal>
#include <QList>
#include <QStringList>

#include "IException.h"
#include "CropTransform.h"
#include "IString.h"

namespace Isis {

/* Constructor */
CropTransform::CropTransform() : GenericTransform("CropTransform"), m_crop() {
}

/* Constructor with name of transform */
CropTransform::CropTransform(const QString &name) : GenericTransform(name),
                                                    m_crop() { }
/* Destructor */
CropTransform::~CropTransform() { }

/**
 * @brief Construct a full crop transform with specified region
 *
 * This constructor will create an object that will crop a specified region of
 * an image.
 *
 * @param name    Name of the transform
 * @param orgSize Original size of the image
 * @param tfSize  New size and starting line/samples of image
 * @param tform   Transformation of the crop
 */
CropTransform::CropTransform(const QString &name, const cv::Size &orgSize,
                             const cv::Size &tfSize, const cv::Mat &tform) :
                             GenericTransform(name, tform, orgSize) {

  m_crop = findCrop(tform, orgSize, tfSize);
  setMatrix(translation(-m_crop.x, -m_crop.y));
  setSize(m_crop.size());

#if 0
  std::cout << "ComputedCropInverse: " << getInverse() << "\n";
  cv::Mat inv = translation(m_crop.x, m_crop.y);
  std::cout << "DirectCropInverse:  " << inv << "\n";
#endif
}

/**
 * @brief Crop and image as specified in as a region
 *
 * The starting coordinates of the image region and size are provided to create
 * a crop transform.
 *
 * @param name   Name of transform
 * @param region Region of the image to crop
 */
CropTransform::CropTransform(const QString &name,
                             const CropTransform::RectArea &region) :
                             GenericTransform(name), m_crop(region) {
  setMatrix(translation(-m_crop.x, -m_crop.y));

#if 0
  std::cout << "ComputedCropInverse: " << getInverse() << "\n";
  cv::Mat inv = translation(m_crop.x, m_crop.y);
  std::cout << "DirectCropInverse:  " << inv << "\n";
#endif
}

/**
 * @brief Crop the input image as specfied in the contructor
 *
 * @param image Image to crop
 *
 * @return cv::Mat Cropped region of the image
 */
cv::Mat CropTransform::render(const cv::Mat &image) const {

  cv::Mat subImage = image(m_crop);
  return ( subImage );
}


/**
 * @brief Determine the crop elements of a given specification
 *
 * @param tform  Matrix specification of the transform
 * @param imSize Input image size
 * @param tfSize Desired size of the cropped image
 *
 * @return CropTransform::RectArea Specs of the region of the cropped image
 */
CropTransform::RectArea CropTransform::findCrop(const cv::Mat &tform,
                                                const cv::Size &imSize,
                                                const cv::Size &tfSize) const {

  std::vector<cv::Point2f> t_corners;
  cv::perspectiveTransform(corners(imSize), t_corners, tform);

  double xmin(qMax(0.0f, t_corners[0].x)), xmax(xmin);
  double ymin(qMax(0.0f, t_corners[0].y)), ymax(ymin);
  for (unsigned int i = 1 ; i < t_corners.size() ; i++) {
    if ( t_corners[i].x < xmin) xmin = t_corners[i].x;
    if ( t_corners[i].x > xmax) xmax = t_corners[i].x;
    if ( t_corners[i].y < ymin) ymin = t_corners[i].y;
    if ( t_corners[i].y > ymax) ymax = t_corners[i].y;
  }

  xmin = qMax(0.0, xmin);
  xmax = qMin(xmax, tfSize.width-1.0);
  ymin = qMax(0.0, ymin);
  ymax = qMin(ymax, tfSize.height-1.0);


  RectArea subarea(xmin, ymin, (int) (xmax-xmin+0.5), (int) (ymax-ymin+0.5) );
  return ( subarea );

}

} // namespace Isis
