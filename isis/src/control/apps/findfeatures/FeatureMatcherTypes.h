#ifndef FeatureMatcherTypes_h
#define FeatureMatcherTypes_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <vector>

#include <QDebug>
#include <QSharedPointer>
#include <QList>

#include <opencv2/opencv.hpp>

#include "ImageSource.h"

// Type definitions for feature matcher algorithms
typedef std::vector<cv::KeyPoint> Keypoints;
typedef std::vector<Keypoints>    KeypointList;
typedef cv::Mat                   Descriptors;
typedef std::vector<Descriptors>  DescriptorList;
typedef std::vector<cv::DMatch>   Matches;

namespace Isis {
  typedef ImageSource                     ImageSourceType;
  typedef QSharedPointer<ImageSourceType> SharedImageSourceType;
  typedef QList<SharedImageSourceType>    ImageSourceTypeList;
}

#endif
