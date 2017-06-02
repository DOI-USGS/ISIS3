#ifndef SobelTransform_h
#define SobelTransform_h
/**
 * @file
 * $Revision$ 
 * $Date$ 
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
 * @brief Apply a Sobel transform the image
 *  
 * @see 
 *      http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html
 *  
 * @author 2015-10-14 Kris Becker 
 * @internal 
 *   @history 2015-10-14 Kris Becker - Original Version 
 */

class SobelTransform : public ImageTransform {
  public:
    SobelTransform() : ImageTransform("SobelTransform"), m_reduceNoise(true) { } 
    SobelTransform(const QString &name, const bool reduceNoise = true) : 
                   ImageTransform(name), m_reduceNoise(reduceNoise) { } 
    virtual ~SobelTransform() { }

    virtual cv::Mat render(const cv::Mat &image) const {
      int scale = 1;
      int delta = 0;
      int ddepth = CV_16S;

      // Initialize and reduce noise if requested
      cv::Mat src = image;
      if ( m_reduceNoise ) {
        cv::GaussianBlur(src, src, cv::Size(3, 3), 0, 0, cv::BORDER_REFLECT ); 
      }

      /// Generate grad_x and grad_y
      cv::Mat grad_x, grad_y;
      cv::Mat abs_grad_x, abs_grad_y;

      /// Gradient X
      cv::Sobel( src, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_REFLECT );
      cv::convertScaleAbs( grad_x, abs_grad_x );

      /// Gradient Y
      cv::Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_REFLECT) ;
      cv::convertScaleAbs( grad_y, abs_grad_y );

      /// Total Gradient (approximate)
      cv::Mat grad;
      cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

      return ( grad) ;
    }

  private:
    bool m_reduceNoise;

};

}  // namespace Isis
#endif
