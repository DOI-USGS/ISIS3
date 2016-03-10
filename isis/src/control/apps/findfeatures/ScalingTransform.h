#ifndef ScalingTransform_h
#define ScalingTransform_h
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
