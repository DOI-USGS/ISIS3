#ifndef ImageSource_h
#define ImageSource_h
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
#include <QMutex>
#include <QString>
#include <QScopedPointer>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>

#include <opencv2/opencv.hpp>

#include "Camera.h"
#include "Cube.h"
#include "SurfacePoint.h"
#include "TProjection.h"

namespace Isis {
  class Camera;
  class Histogram;

/**
 * @brief Provides Image I/O and geometry support for matching purposes 
 *  
 * This class provides I/O and geometry support that is tailored to the ISIS 
 * environment but not limited to it.  It will also read any format supported by 
 * the OpenCV image interfaces. 
 *  
 * @author 2014-07-01 Kris Becker 
 * @internal 
 *   @history 2014-07-01 Kris Becker - Original Version 
 *   @history 2016-02-08 Kris Becker - Changed call to ISIS Histogram class for 
 *                           recent changes
 */

class ImageSource {
  public:
    ImageSource();
    ImageSource(const QString &name, 
                const bool geometryOnly = false, 
                const double minPercent = 0.5, 
                const double maxPercent = 99.5);
    explicit ImageSource(const QString &name, const cv::Mat &image,
                         const QString &identifier = "");
    ImageSource(const ImageSource &other, const bool getGeometry = false);

    virtual ~ImageSource() { }

    void load(const double minPercent = 0.5,const double maxPercent = 99.5); 
    void load(const QString &name, const double minPercent = 0.5,
              const double maxPercent = 99.5); 

    inline QString name() const {  return ( m_data->m_name );  }
    inline QString serialno() const { return (  m_data->m_serialno ); }

    inline int samples() const {  return (  m_data->m_image.cols ); }
    inline int lines()   const {  return (  m_data->m_image.rows ); }

    bool hasGeometry() const;
    bool hasProjection() const;
    bool hasCamera() const;

    virtual cv::Mat image() const {  return (  m_data->m_image );  }

    QString getTargetName() const;

    SurfacePoint getLatLon(const double &line, const double &sample);
    bool getLineSamp(const SurfacePoint &point,
                     double &line, double &samp, double &radius);
    cv::Mat getGeometryMapping(ImageSource &match, const int &minpts = 25, 
                               const double &tol = 3.0,
                               const cv::Rect &subarea = cv::Rect());



  private:
    /** 
     *  Shared Image data pointer
     *  
     * @author 2014-07-01 Kris Becker 
     * @internal 
     *   @history 2014-07-01 Kris Becker - Original Version 
     */
    class SourceData : public QSharedData {
      public:
        SourceData() : m_name("Image"), m_serialno("none"), m_projection(0),
                       m_camera(0), m_image(), m_mutex(new QMutex()) { }
        SourceData(const QString &name) : m_name(name), m_serialno("none"), 
                                          m_projection(0),m_camera(0), 
                                          m_image(), m_mutex(new QMutex()) { }
        SourceData(const QString &name, const cv::Mat &image, 
                   const QString &serialno) : m_name(name), m_serialno(serialno),
                                              m_projection(0), m_camera(0), 
                                              m_image(image), m_mutex(new QMutex()) { }
        SourceData(const QString &name, const QString &serialno,
                   TProjection *proj, Camera *camera, const cv::Mat &image) : 
                   m_name(name), m_serialno(serialno),
                   m_projection(proj), m_camera(camera), 
                   m_image(image), m_mutex(new QMutex()) { }
        SourceData(const SourceData &other) : QSharedData(other), 
                                              m_name(other.m_name),
                                              m_serialno(other.m_serialno),
                                              m_projection(0),
                                              m_camera(0), 
                                              m_image(), 
                                              m_mutex( new QMutex() ) { }
                                               
        ~SourceData() {
          delete m_projection;
          delete m_camera;
          delete m_mutex;
        }

        // Data....
        QString     m_name;
        QString     m_serialno;

        TProjection *m_projection;
        Camera      *m_camera;

        cv::Mat     m_image;

        QMutex      *m_mutex;    //!< Mutex for thread saftey
    };

    QExplicitlySharedDataPointer<SourceData> m_data;

    Histogram *getHistogram(Cube &cube) const;

    bool initGeometry();
    bool initGeometry(Cube &cube);
    
};


typedef QSharedPointer<ImageSource> SharedImageSource;
typedef QList<SharedImageSource>    ImageSourceList;

}  // namespace Isis
#endif
