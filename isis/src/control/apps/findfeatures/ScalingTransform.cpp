/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */ 


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
