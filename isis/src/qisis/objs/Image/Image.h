
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
  class ImageDisplayProperties;
  class Project;
  class PvlObject;

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
   *   @history 2015-10-14 Jeffrey Covington - Declared Image * as a Qt
   *                           metatype for use with QVariant.
   *   @history 2014-09-05 Kimberly Oyama - Added the serialNumber() function which returns
   *                           the cube's serial number.
   *   @history 2015-09-05 Kenneth Edmundson - Added preliminary target body information
   *                           (re:  the member variables QString m_instrumentId (the instrument ID
   *                           of the image), SpiceInt * m_bodyCode (the NaifBodyCode value if it
   *                           exists in the labels), and QString m_spacecraftName
   *                           (the Spacecraft name associated with this image).
   *   @history 2016-06-22 Tyler Wilson - Added documentation to member functions/variables.
   *                           Fixes #3950.
   *   @history 2017-10-11 Summer Stapleton - Removed path to instrumentId and spacecraftName in
   *                           the startElement method. Fixes #5179.
   *   @history 2017-11-01 Tracie Sucharski - Changed copyToNewProjectRoot to handle Images that are
   *                           located outside of the import image directories such as the updated
   *                           Images from a bundle run.  To improve efficiency, return from method
   *                           if the project root has not changed. Fixes #4849.
   *   @history 2018-06-30 Ian Humphrey - Added observationNumber() method so anything that grabs
   *                           an Image ProjectItem can easily get both the serial number and
   *                           observation number now. References #497.
   *   @history 2018-07-02 Ian Humphrey - Changed serialNumber() implementation to follow how
   *                           observationNumber() is implemented. This ensures that any calls
   *                           after the first call to these methods are O(1) and are not
   *                           bottlenecekd by any file I/O that occurs in the Compose()
   *                           methods. References #497.
   *   @history 2018-10-03 Tracie Sucharski - Added constructor which takes cube and a calculated
   *                           footprint.  This was done for ipce imported shapes which do not
   *                           contain a footprint. References #5504.
   */

  class Image : public QObject {
    Q_OBJECT
    public:
      explicit Image(QString imageFileName, QObject *parent = 0);
      explicit Image(Cube *imageCube, QObject *parent = 0);
      explicit Image(Cube *imageCube, geos::geom::MultiPolygon *footprint, QString id,
                     QObject *parent = 0);
      ~Image();

      void fromPvl(const PvlObject &pvl);
      PvlObject toPvl() const;

      bool isFootprintable() const;
      Cube *cube();
      void closeCube();
      ImageDisplayProperties *displayProperties();
      const ImageDisplayProperties *displayProperties() const;
      QString fileName() const;
      QString observationNumber();
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
      geos::geom::MultiPolygon *createFootprint(QMutex *cameraMutex);
      void initCamStats();
      void initQuickFootprint();

    private:
      Image(const Image &other);
      Image &operator=(const Image &rhs);

      SpiceInt *m_bodyCode;    /**< The NaifBodyCode value, if it exists in the
                                    labels. Otherwise, if the target is sky,
                                    it's the SPK code and if not sky then it's
                                    calculated by the NaifBodyCode() method.*/
      //    QString *m_name;   //!< Name of the target


      /**
       * The cube associated with this Image. This is usually NULL once the image is done
       * initializing because no more than a thousand of these should ever be open at once.
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
       * Instrument id associated with this Image.
       */
      QString m_instrumentId;

      /**
       * The observation number for this image.
       */
      QString m_observationNumber; 

      /**
       * The serial number for this image.
       */
      QString m_serialNumber;

      /**
       * Spacecraft name associated with this Image.
       */
      QString m_spacecraftName;

      /**
       * A 0-360 ocentric lon,lat degrees footprint of this Image.
       */
      geos::geom::MultiPolygon *m_footprint;
      /**
       * A unique ID for this Image (useful for others to reference this Image when saving to disk).
       */
      QUuid *m_id;

      double m_aspectRatio;         //!<  Aspect ratio of the image.
      double m_resolution;          //!<  Resolution of the image.
      Angle m_emissionAngle;        //!<  Emmission angle of the image.
      Angle m_incidenceAngle;       //!<  Incidence angle of the image.
      double m_lineResolution;      //!<  Line resolution of the image.
      double m_sampleResolution;    //!<  Sample resolution of the image.
      Distance m_localRadius;       //!<  Local radius of the image.
      Angle m_northAzimuth;         //!<  North Azimuth for the image.
      Angle m_phaseAngle;           //!<  Phase angle for the image.

  };
  // TODO: add QDataStream >> and << ???
}

Q_DECLARE_METATYPE(Isis::Image *);

#endif
