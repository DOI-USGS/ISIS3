#ifndef ImageSource_h
#define ImageSource_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
#include "Projection.h"
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
 *   @history 2019-11-19 Kris Becker - Correctly return of target name from the
 *                           Mapping group of projected images. Incorrect
 *                           keyword Target used instead of TargetName
 *   @history 2023-08-03 Kris J. Becker - Use Projection rather than TProjection
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
     *   @history 2023-08-03 Kris J. Becker  Use Projection rather than
     *                         TProjection; add getters
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
                   Projection *proj, Camera *camera, const cv::Mat &image) :
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

        QString name() const {
          return ( m_name );
        }

        QString serialnumber() const {
          return ( m_serialno );
        }

        TProjection *projection() const {
          return ( (TProjection *) m_projection );
        }

        Camera *camera() const {
          return ( m_camera );
        }

        // Data....
        QString     m_name;
        QString     m_serialno;

        Projection  *m_projection;
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
