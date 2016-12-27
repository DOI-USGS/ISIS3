#ifndef CropTransform_h
#define CropTransform_h
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

#include <QList>
#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

#include "FeatureMatcherTypes.h"
#include "ImageTransform.h"
#include "GenericTransform.h"

namespace Isis {

/**
 * @brief Crop a section from an image using a translation (affine) transform
 *  
 * This image transform will crop a section of an image stored in an OpenCV 
 * matrix and return the cropped version of the image. 
 *  
 * @author 2015-10-01 Kris Becker 
 * @internal 
 *   @history 2015-10-01 Kris Becker - Original Version 
 */

class CropTransform : public GenericTransform {
  public:
    typedef GenericTransform::RectArea   RectArea;
    CropTransform();
    CropTransform(const QString &name);
    CropTransform(const QString &name, const cv::Size &orgSize, 
                  const cv::Size &tfSize, const cv::Mat &tform); 
    CropTransform(const QString &name, const RectArea &region);
    virtual ~CropTransform();

    virtual cv::Mat render(const cv::Mat &image) const;

  protected:
    cv::Rect findCrop(const cv::Mat &tform, const cv::Size &imSize,
                      const cv::Size &tfSize) const;

  private:
    RectArea    m_crop;
};

}  // namespace Isis
#endif
