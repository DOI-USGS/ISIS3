#ifndef GuiCamera_h
#define GuiCamera_h

/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2009/10/15 01:35:17 $
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

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>

//#include "Angle.h"
//#include "Distance.h"
#include "XmlStackedHandler.h"

#include <vector>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  //class Distance;
  class FileName;
  class Project;  // TODO: does xml stuff need project???
  class PvlObject;
  class Camera;
  class GuiCameraDisplayProperties;
  class XmlStackedHandlerReader;

  /**
   * @brief Container class for GuiCamera.
   *
   * This class represents a camera in a project-based GUI interface. It encapsulates ideas
   *   about a camera such as it's display name, how it should be viewed, where it is on disk, etc.
   *
   *  
   * @ingroup qisis
   *
   * @author 2015-06-23 Ken Edmundson
   *
   * @internal
   *   @history 2015-06-23 Ken Edmundson - Original version.
   *  
   */
  class GuiCamera : public QObject {
    Q_OBJECT
    public:
      GuiCamera(Camera *camera, QObject *parent = 0);
//      GuiCamera(Project *project, XmlStackedHandlerReader *xmlReader,
//                 QObject *parent = 0);  // TODO: does xml stuff need project???
      ~GuiCamera();

      bool operator==(const GuiCamera &srcGuiCamera) const;

      GuiCameraDisplayProperties *displayProperties();
      const GuiCameraDisplayProperties *displayProperties() const;

      QString id() const;

//      Camera *camera();

      QString instrumentNameShort();
      QString instrumentNameLong();
      QString spacecraftNameShort();
      QString spacecraftNameLong();

//    void deleteFromDisk();

//      SpiceInt naifBodyCode() const;
//      Distance radiusA() const;
//      Distance radiusB() const;
//      Distance radiusC() const;
//      Distance meanRadius() const;
//      Distance sigmaRadiusA() const;
//      Distance sigmaRadiusB() const;
//      Distance sigmaRadiusC() const;
//      Distance sigmaMeanRadius() const;

//      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;  // TODO: does xml stuff need project and newRoot???
//      void save(QXmlStreamWriter &stream, const Project *project) const;  // TODO: does xml stuff need project???

//      QDataStream &write(QDataStream &stream) const;
//      QDataStream &read(QDataStream &stream);

//      void savehdf5(FileName outputfilename) const;

    private:
      /**
       *
       * @author 2015-06-08 Ken Edmundson
       *
       * @internal
       */
//      class XmlHandler : public XmlStackedHandler {
//        public:
//          XmlHandler(GuiCamera *GuiCamera, Project *project);  // TODO: does xml stuff need project???
//          ~XmlHandler();

//          virtual bool startElement(const QString &namespaceURI, const QString &localName,
//                                    const QString &qName, const QXmlAttributes &atts);
//          virtual bool characters(const QString &ch);
//          virtual bool endElement(const QString &namespaceURI, const QString &localName,
//                                    const QString &qName);

//        private:
//          Q_DISABLE_COPY(XmlHandler);

//          GuiCamera *m_xmlHandlerGuiCamera;
//          Project *m_xmlHandlerProject;  // TODO: does xml stuff need project???
//          QString m_xmlHandlerCharacters;
//      };

    private:
      GuiCamera(const GuiCamera &other);          // NOTE: copy constructor & assignment operators
      GuiCamera &operator=(const GuiCamera &src);   // are private so compiler will generate error
                                                    // if trying to use them (because parent is
                                                    // QObject which uses Q_DISABLE_COPY macro

      /**
       * A unique ID for this GuiCamera object (useful for others to reference this object
       *   when saving to disk).
       */
      QUuid *m_id;

      /**
       * Camera
       */
//      Camera *m_camera;

      /**
       * target radii sigmas
       */
//      std::vector<Distance> m_sigmaRadii;

      /**
       * The GUI information for how this camera will be displayed.
       */
      GuiCameraDisplayProperties *m_displayProperties;

      QString m_spacecraftNameShort;
      QString m_spacecraftNameLong;
      QString m_instrumentNameShort;
      QString m_instrumentNameLong;
  };

  typedef QSharedPointer<GuiCamera> GuiCameraQsp;

  // operators to read/write GuiCamera to/from binary data
  QDataStream &operator<<(QDataStream &stream, const GuiCamera &GuiCamera);
  QDataStream &operator>>(QDataStream &stream, GuiCamera &GuiCamera);
};
#endif // GuiCamera_h
