#ifndef GenericTransform_h
#define GenericTransform_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


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
 *   @history 2021-10-30 Kris J. Becker Added verify parameter to
 *                           calculateInverse() method; if verify==true, the
 *                           matrix is tested if it is indeed
 *                           invertable.
 *   @history 2022-02-07 Kris Becker Modifications in response to code review
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

    static cv::Mat computeInverse(const cv::Mat &matrix,
                                  const bool verify = true);

  protected:
    void setMatrix(const cv::Mat &matrix);
    void setInverse(const cv::Mat &matrix);


    // Use in deriving clases for differnt behavior
    virtual cv::Mat calculateInverse(const cv::Mat &matrix,
                                     const bool verify = true);

    void setSize(const cv::Size &mSize);

  private:
    cv::Mat  m_matrix;    //!< Generic tranform matrix
    cv::Mat  m_inverse;   //!< Inverse of m_matrix
    cv::Size m_size;      //!< Optional output size of resuting image
};

}  // namespace Isis
#endif
