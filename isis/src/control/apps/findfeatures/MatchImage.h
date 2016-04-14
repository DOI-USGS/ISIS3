#ifndef MatchImage_h
#define MatchImage_h
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
#include <QSharedData>
#include <QExplicitlySharedDataPointer>

#include <opencv2/opencv.hpp>

#include "FeatureMatcherTypes.h"
#include "ImageSource.h"
#include "ImageTransform.h"
#include "Transformer.h"

namespace Isis {

/**
 * @brief Container for match image data
 *  
 * This class provides storage and computational conversion from original 
 * source image to transformed image. The transformed image takes the original 
 * source image and applies any image transformations to render the image that 
 * is to used in the matcher. 
 *  
 * Its internal data storage is explicitly shared so the object can be copied 
 * freely. When a new image is set, the match data is cleared including the 
 * transforms for clean match rendering.
 *  
 * @author 2015-10-03 Kris Becker 
 * @internal 
 *   @history 2015-10-03 Kris Becker - Original Version 
 */

class MatchImage {
  public:
    MatchImage() : m_data( new ImageData() ) { }

    MatchImage(const ImageSource &source) {
      m_data = new ImageData();
      m_data->m_source = source;
    }

    MatchImage(const ImageSource &source, 
               const Keypoints &keypoints, 
               const Descriptors &descriptor,
               const double &ptime = 0.0)  {
      m_data = new ImageData();
      m_data->m_source = source;
      m_data->m_keypoints = keypoints;
      m_data->m_descriptors = descriptor;
      m_data->m_duration = ptime;
    }



    virtual ~MatchImage() { }

    inline int size() const {
      return ( m_data->m_keypoints.size() );
    }

    inline QString name() const {
      return ( m_data->m_source.name());
    }

    inline QString id() const {
      return ( m_data->m_source.serialno() );
    }

    inline QString target() const {
      return ( m_data->m_source.getTargetName() );
    }

    inline void setSource(const ImageSource &source) {
      m_data.detach();
      m_data->m_source = source;
    }

    inline void addTransform(ImageTransform *transform) {
      m_data->m_transforms.add(transform);
    }

    inline void clearTransforms() {
      m_data->m_transforms.clear();
    }

    inline ImageSource &source() const {
      return ( m_data->m_source );
    }

    inline const cv::Mat image() const {
      // Run through transforms to render image
      return ( m_data->m_transforms.render( m_data->m_source.image()) );
    }

    inline const Keypoints &keypoints() const {
      return ( m_data->m_keypoints );
    }

    inline Keypoints &keypoints() {
      return ( m_data->m_keypoints );
    }

    inline const cv::KeyPoint &keypoint(const int &index) const {
      Q_ASSERT ( index < (int) m_data->m_keypoints.size() );
      Q_ASSERT ( index >= 0 );
      return ( m_data->m_keypoints.at(index) );
    }

    inline void addTime(const double &delta) {
      m_data->addTime( delta );
      return;
    }

    inline double time() const {
      return ( m_data->m_duration );
    }

    inline void setDescriptors(const Descriptors &descriptors) {
      m_data->m_descriptors = descriptors;
    }

    inline const Descriptors &descriptors() const {
      return ( m_data->m_descriptors );
    }

    inline Descriptors &descriptors() {
      return ( m_data->m_descriptors );
    }

    inline cv::Point2f imageToSource(const cv::Point2f &point) const {
      return ( m_data->m_transforms.inverse(point) );
    }

    inline cv::Point2f sourceToImage(const cv::Point2f &point) {
      return ( m_data->m_transforms.forward(point) );
    }

  private:
    /** 
     *  Shared Image data pointer
     *  
     * @author 2015-10-03 Kris Becker 
     * @internal 
     *   @history 2015-10-03 Kris Becker - Original Version 
     */
    class ImageData : public QSharedData {
      public:
        ImageData() : m_source(), m_transforms(),
                      m_keypoints(), m_descriptors(), 
                      m_duration(0) { }
        ImageData(const ImageData &other) : QSharedData(other),
                                            m_source(other.m_source), 
                                            m_transforms(), 
                                            m_keypoints(), 
                                            m_descriptors(), 
                                            m_duration(0) { }
        ~ImageData() { }

        inline void addTime(const double &delta) {
          m_duration += delta;
        }

        // Data....
        ImageSource  m_source; 
        Transformer  m_transforms;
        Keypoints    m_keypoints;
        Descriptors  m_descriptors;
        double       m_duration;
    };
    QExplicitlySharedDataPointer<ImageData> m_data;


};

///!<   Match image list declaration
typedef QList<MatchImage>    MatchImageQList;


}  // namespace Isis
#endif
