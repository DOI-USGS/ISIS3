
#ifndef Shape_H
#define Shape_H
/**
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
#include "Latitude.h"
#include "Longitude.h"
#include "XmlStackedHandler.h"

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

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
  class Image;
  class Project;
  class PvlObject;
  class ShapeDisplayProperties;
  class XmlStackedHandlerReader;

  /**
   * This represents a shape in a project-based GUI interface. The actual cube doesn't have to be
   *   open. This encapsulates ideas about an shape such as it's footprint, it's cube, how it
   *   should be viewed, where it is on disk, etc. This class is designed to be semi-light weight -
   *   we should be able to have tens of thousands of these in memory without any issues (provided
   *   the Cube files aren't open - see closeCube()).
   *
   * @author 2016-07-25 Tracie Sucharski
   *
   * @internal 
   *   @history 2016-10-21 Tracie Sucharski - Add Image to the Shape class.  This was done because
   *                          the qmos class expect Images, and we want to display a footprint of
   *                          a Shape.
   */
  class Shape : public QObject {
    Q_OBJECT
    public:
      enum ShapeType {
        Dem,
        Basemap,
        Unprojected,
        Unknown };

      explicit Shape(QString shapeFileName, QObject *parent = 0);
      explicit Shape(Cube *shapeCube, QObject *parent = 0);
      Shape(FileName shapeFolder, XmlStackedHandlerReader *xmlReader, QObject *parent = 0);
      ~Shape();

      void fromPvl(const PvlObject &pvl);
      PvlObject toPvl() const;

      bool isFootprintable() const;
//    bool hasImage() const;
//    Image *image();
      Cube *cube();
      void closeCube();
      ShapeType shapeType();
      ShapeDisplayProperties *displayProperties();
      const ShapeDisplayProperties *displayProperties() const;
      QString fileName() const;
      QString serialNumber();
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
      Shape(const Shape &other);
      Shape &operator=(const Shape &rhs);

      void initMemberData();
      void initShape();
      void initCamStats();
      void initMapStats();
      void initDemStats();

      geos::geom::MultiPolygon *createFootprint(QMutex *cameraMutex);
      void initQuickFootprint();

      /**
       *
       * @author 2012-??-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(Shape *shape, FileName shapeFolder);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          Shape *m_shape;
          FileName m_shapeFolder;
          QString m_characters;
      };

    private:
      SpiceInt *m_bodyCode;    /**< The NaifBodyCode value, if it exists in the
                                    labels. Otherwise, if the target is sky,
                                    it's the SPK code and if not sky then it's
                                    calculated by the NaifBodyCode() method.*/
//    QString *m_name;   //!< Name of the target


      /**
       * The cube associated with this Shape. This is usually NULL once the shape is done
       *   initializing because no more than a thousand of these should ever be open at once.
       */
      Cube *m_cube;

      ShapeType m_shapeType;

      /**
       * The GUI information for how this Shape ought to be displayed.
       */
      ShapeDisplayProperties *m_displayProperties;
      /**
       * The on-disk file name of the cube associated with this Shape.
       */
      QString m_fileName;
      /**
       * Instrument id associated with this Shape.
       */
      QString m_instrumentId;
      /**
       * Spacecraft name associated with this Shape.
       */
      QString m_spacecraftName;
      /**
       * A 0-360 ocentric lon,lat degrees footprint of this Shape.
       */
      geos::geom::MultiPolygon *m_footprint;
      /**
       * A unique ID for this Shape (useful for others to reference this Shape when saving to disk).
       */
      QUuid *m_id;

      // Level 1 labels
      double m_aspectRatio;
      double m_resolution;
      Angle m_emissionAngle;
      Angle m_incidenceAngle;
      double m_lineResolution;
      Distance m_localRadius;
      Angle m_northAzimuth;
      Angle m_phaseAngle;
      double m_sampleResolution;

      // Mapping labels
      QString m_targetName;
      QString m_projectionName;
      Longitude m_centerLongitude;
      Latitude m_centerLatitude;
      Latitude m_minimumLatitude;
      Latitude m_maximumLatitude;
      Longitude m_minimumLongitude;
      Longitude m_maximumLongitude;
      double m_pixelResolution;
      double m_scale;

      // Dem Labels
      Distance m_minimumRadius;
      Distance m_maximumRadius;
  };

}

Q_DECLARE_METATYPE(Isis::Shape *);

#endif
