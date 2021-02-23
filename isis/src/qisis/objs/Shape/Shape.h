#ifndef Shape_H
#define Shape_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QObject>

#include <QString>

#include "Angle.h"
#include "ControlPoint.h"
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
   *   @history 2018-09-10 Tracie Sucharski - Added surface point source and radius source along
   *                          with access methods.
   *   @history 2018-09-21 Tracie Sucharski - Serial number is composed on construction and is
   *                          always the filename.
   *   @history 2018-10-03 Tracie Sucharski - Fixed problem in constructor with cube being
   *                          set before member data was initialized. Fixed the computation for
   *                          surfacePointSource and radiusSource. References #5504.
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
      Cube *cube();
      void closeCube();
      ControlPoint::SurfacePointSource::Source surfacePointSource();
      ControlPoint::RadiusSource::Source radiusSource();
      ShapeDisplayProperties *displayProperties();
      ShapeType shapeType();
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
      ControlPoint::SurfacePointSource::Source m_surfacePointSource;
      ControlPoint::RadiusSource::Source m_radiusSource;
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
       * This will always be simply the filename and is created on construction.
       */
      QString m_serialNumber;
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
