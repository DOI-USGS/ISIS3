#ifndef FeatureMatcherTypes_h
#define FeatureMatcherTypes_h
/**
 * @file
 * $Revision: 6563 $ 
 * $Date: 2016-02-10 16:56:52 -0700 (Wed, 10 Feb 2016) $ 
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
