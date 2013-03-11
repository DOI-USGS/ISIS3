
#ifndef Image_H
#define Image_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <QObject>

#include <QString>

#include "Angle.h"
#include "Distance.h"
#include "FileName.h"
#include "XmlStackedHandler.h"

class QUuid;
class QMutex;
class QXmlStreamWriter;

namespace geos {
  namespace geom {
    class MultiPolygon;
  }
}

namespace Isis {
  class Cube;
  class FileName;
  class ImageDisplayProperties;
  class Project;
  class PvlObject;
  class XmlStackedHandlerReader;

  /**
   * This represents a cube in a project-based GUI interface. The actual cube doesn't have to be
   *   open. This encapsulates ideas about an image such as it's footprint, it's cube, how it
   *   should be viewed, where it is on disk, etc. This class is designed to be semi-light weight -
   *   we should be able to have tens of thousands of these in memory without any issues (provided
   *   the Cube files aren't open - see closeCube()).
   *
   * @author 2012-07-23 ???
   *
   * @internal
   *   @history 2011-05-11 Steven Lambright - Added accessors for data that is
   *                           complicated to get or expensive (i.e. Camera
   *                           statistics and the footprint).
   *   @history 2011-05-18 Steven Lambright - Fixed the second constructor
   *   @history 2012-10-02 Steven Lambright - Added support for camera statistics information that
   *                           used to be in CubeDisplayProperties (a class that no longer exists).
   *   @history 2012-10-04 Jeannie Backer Changed references to TableField
   *                           methods to lower camel case. Fixed history
   *                           entry indentation. Added padding to control
   *                           statements. References #1169.
   */
  class Image : public QObject {
    Q_OBJECT
    public:
      explicit Image(QString imageFileName, QObject *parent = 0);
      explicit Image(Cube *imageCube, QObject *parent = 0);
      Image(FileName imageFolder, XmlStackedHandlerReader *xmlReader, QObject *parent = 0);
      ~Image();

      void fromPvl(const PvlObject &pvl);
      PvlObject toPvl() const;

      bool isFootprintable() const;
      Cube *cube();
      void closeCube();
      ImageDisplayProperties *displayProperties();
      const ImageDisplayProperties *displayProperties() const;
      QString fileName() const;
      geos::geom::MultiPolygon *footprint();
      const geos::geom::MultiPolygon *footprint() const;
      void setId(QString id);

      bool initFootprint(QMutex *cameraMutex);

      double aspectRatio() const;
      QString id() const;
      double resolution() const;
      Angle emissionAngle() const;
      Angle incidenceAngle() const;
      double lineResolution() const;
      Distance localRadius() const;
      Angle northAzimuth() const;
      Angle phaseAngle() const;
      double sampleResolution() const;

      void copyToNewProjectRoot(const Project *project, FileName newProjectRoot);
      void deleteFromDisk();
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;

    public slots:
      void updateFileName(Project *);

    private:
      geos::geom::MultiPolygon *createFootprint(QMutex *cameraMutex);
      void initCamStats();
      void initQuickFootprint();

    private:
      /**
       *
       * @author 2012-??-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(Image *image, FileName imageFolder);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          Image *m_image;
          FileName m_imageFolder;
          QString m_characters;
      };

    private:
      Image(const Image &other);
      Image &operator=(const Image &rhs);

      /**
       * The cube associated with this Image. This is usually NULL once the image is done
       *   initializing because no more than a thousand of these should ever be open at once.
       */
      Cube *m_cube;
      /**
       * The GUI information for how this Image ought to be displayed.
       */
      ImageDisplayProperties *m_displayProperties;
      /**
       * The on-disk file name of the cube associated with this Image.
       */
      QString m_fileName;
      /**
       * A 0-360 ocentric lon,lat degrees footprint of this Image.
       */
      geos::geom::MultiPolygon *m_footprint;
      /**
       * A unique ID for this Image (useful for others to reference this Image when saving to disk).
       */
      QUuid *m_id;

      double m_aspectRatio;
      double m_resolution;
      Angle m_emissionAngle;
      Angle m_incidenceAngle;
      double m_lineResolution;
      Distance m_localRadius;
      Angle m_northAzimuth;
      Angle m_phaseAngle;
      double m_sampleResolution;
  };
}

#endif
