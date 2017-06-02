#ifndef Transformer_h
#define Transformer_h
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

namespace Isis {

/**
 * @brief Provide a series of image transforms
 *  
 * This class serves as a container class for all image transformations and 
 * point conversions, both forward and inverse.
 *  
 * As each image is read, the list of transforms are applied for each transform 
 * added to the this class. There is typically a list of transforms for each 
 * image. 
 *  
 * @author 2015-10-03 Kris Becker 
 * @internal 
 *   @history 2015-10-03 Kris Becker - Original Version 
 *   @history 2016-04-05 Kris Becker Completed documentation 
 */

  class Transformer {
    public:
      typedef ImageTransformQList::iterator       ImageTransformIterator;
      typedef ImageTransformQList::const_iterator ImageTransformConstIterator;
  
      Transformer();
      virtual ~Transformer();
  
      int size() const;
  
      void add(ImageTransform *transform);
      cv::Mat render(const cv::Mat &image) const;
  
      cv::Point2f forward(const cv::Point2f &point) const;
      cv::Point2f inverse(const cv::Point2f &point) const;
  
      ImageTransformConstIterator begin() const;
      ImageTransformConstIterator end() const;
  
      void clear();
      
    private:
      ImageTransformQList m_transforms;  //!<  List of transforms
  };

}  // namespace Isis
#endif
