#ifndef ScharrTransform_h
#define ScharrTransform_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>
#include <boost/foreach.hpp>

#include "ImageTransform.h"

namespace Isis {

/**
 * @brief Apply a Sobel transform the image
 *
 *  @see
 *       http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html
 *
 * @author 2015-10-14 Kris Becker
 * @internal
 *   @history 2015-10-14 Kris Becker - Original Version
 *   @history 2017-06-22 Jesse Mapel - Modified render to make a deep copy of
 *                                     the input matrix. References #4904.
 */

class ScharrTransform : public ImageTransform {
  public:
    ScharrTransform() : ImageTransform("ScharrTransform"), m_reduceNoise(true) { }
    ScharrTransform(const QString &name, const bool reduceNoise = true) :
                   ImageTransform(name), m_reduceNoise(reduceNoise) { }
    virtual ~ScharrTransform() { }


    /**
     * Perform the transformation on an image matrix. If the reduce noise flag
     * is set, then this will apply a Gaussian filter with a 3x3 kernel prior
     * to performing the Scharr transformation.
     *
     * @param image The input image data matrix to transform.
     *
     * @return @b cv::Mat The transformed matrix.
     *
     * @note If the reduce noise flag is set, this method creates a deep copy
     *       of the image data matrix which may consume a large amount of memory.
     */
    virtual cv::Mat render(const cv::Mat &image) const {
      int scale = 1;
      int delta = 0;
      int ddepth = CV_16S;

      // Initialize and reduce noise if requested
      //   cv::Mat creates shallow copies by default and does not detach on
      //   modification. So, if applying the Gaussian filter, a deep copy of
      //   the matrix must be created. Otherwise cv::Mat's copy constructor
      //   can be used.
      //   For more details about this see ticket #4904. JAM
      cv::Mat src;
      if ( m_reduceNoise ) {
        src = image.clone();
        cv::GaussianBlur(src, src, cv::Size(3, 3), 0, 0, cv::BORDER_REFLECT );
      }
      else {
        src = image;
      }

      /// Generate grad_x and grad_y
      cv::Mat grad_x, grad_y;
      cv::Mat abs_grad_x, abs_grad_y;

      /// Gradient X
      cv::Scharr( src, grad_x, ddepth, 1, 0, scale, delta, cv::BORDER_REFLECT );
      cv::convertScaleAbs( grad_x, abs_grad_x );

      /// Gradient Y
      cv::Scharr( src, grad_y, ddepth, 0, 1, scale, delta, cv::BORDER_REFLECT );
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
