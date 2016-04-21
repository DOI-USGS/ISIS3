#ifndef GenericTransform_h
#define GenericTransform_h
/**
 * @file
 * $Revision: 6601 $ 
 * $Date: 2016-03-09 10:31:51 -0700 (Wed, 09 Mar 2016) $ 
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

#include "ImageTransform.h"

namespace Isis {

/**
 * @brief Apply a generic transform using a matrix with various options 
 *  
 * This class privides a generic interface to the ImageTransform image and point 
 * transform conversion transform. It supports the transform with a specified 
 * 3x3 matrix and optional sizing operations. 
 *  
 * @author 2014-07-01 Kris Becker 
 * @internal 
 *   @history 2014-07-01 Kris Becker - Original Version 
 *   @history 2016-03-08 Kris Becker Created .cpp from header and completed 
 *                           documentation
 */
class GenericTransform : public ImageTransform {
  public:
    typedef ImageTransform::RectArea   RectArea;
    GenericTransform();
    GenericTransform(const QString &name);
    GenericTransform(const QString &name, const cv::Mat &matrix);
    GenericTransform(const QString &name, const cv::Mat &matrix,
                     const cv::Size &imSize);
    GenericTransform(const QString &name, const cv::Mat &matrix,
                     const RectArea &subarea);

    virtual ~GenericTransform();

    cv::Mat getMatrix() const;
    cv::Mat getInverse() const;
     
    cv::Size getSize(const cv::Mat &image = cv::Mat()) const;

    virtual cv::Mat render(const cv::Mat &image) const;

    virtual cv::Point2f forward(const cv::Point2f &point) const;
    virtual cv::Point2f inverse(const cv::Point2f &point) const;

  protected:
    void setMatrix(const cv::Mat &matrix);
    void setInverse(const cv::Mat &matrix);

    virtual cv::Mat calculateInverse(const cv::Mat &matrix);

    void setSize(const cv::Size &mSize);

  private:
    cv::Mat  m_matrix;    //!< Generic tranform matrix
    cv::Mat  m_inverse;   //!< Inverse of m_matrix
    cv::Size m_size;      //!< Optional output size of resuting image
};

}  // namespace Isis
#endif
