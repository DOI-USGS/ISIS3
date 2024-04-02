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

#include <vector>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>
#include "XmlStackedHandler.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {

  class Camera;
  class FileName;
  class GuiCameraDisplayProperties;
  class Project;  // TODO: does xml stuff need project???
  class PvlObject;  

  /**
   * @brief Container class for GuiCamera.
   *
   * This class represents a camera in a project-based GUI interface. It encapsulates ideas
   * about a camera such as it's display name, how it should be viewed, where it is on disk, etc.
   *
   *  
   * @ingroup qisis
   *
   * @author 2015-06-23 Ken Edmundson
   *
   * @internal
   *   @history 2015-06-23 Ken Edmundson - Original version.
   *   @history 2015-10-14 Jeffrey Covington - Declared GuiCameraQsp as a Qt
   *                           metatype for use with QVariant.
   *   @history 2016-06-08 Tyler Wilson - Added documentation to some functions
   *                           and corrected the formatting.  Fixes #3997.
   *   @hitsory 2018-07-12 Summer Stapleton - Added m_instrumentId and instrumentId() in order to 
   *                           collect the InstrumentId from the original cube label for 
   *                           comparisons related to image imports in ipce. References #5460.
   *  
   */
  class GuiCamera : public QObject {
    Q_OBJECT
    public:
      GuiCamera(Camera *camera, QObject *parent = 0);
      ~GuiCamera();

      bool operator==(const GuiCamera &srcGuiCamera) const;

      //GuiCameraDisplayProperties *displayProperties();
      const GuiCameraDisplayProperties *displayProperties() const;

      QString id() const;

      QString instrumentId();

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

//      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;
//      TODO: does xml stuff need project and newRoot???
//      void save(QXmlStreamWriter &stream, const Project *project) const;
//      TODO: does xml stuff need project???

//      QDataStream &write(QDataStream &stream) const;
//      QDataStream &read(QDataStream &stream);


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
      
      QString m_instrumentId;        //!< The InstrumentId as it appears on the cube.

      QString m_spacecraftNameShort; //!< An abbreviated name for the spacecraft.

      QString m_spacecraftNameLong; //!< The full spacecraft name

      QString m_instrumentNameShort; //!< The abbreviated instrument name

      QString m_instrumentNameLong; //!< The full instrument name
  };

  /**
   * @brief GuiCameraQsp  Represents a smart pointer to a GuiCamera object.
   * It behaves exactly like a normal pointer, but it is thread-safe and it will delete the pointer
   *  it is holding when it goes out of scope, provided no other QSharedPointer objects
   * are referencing it.
   */

  typedef QSharedPointer<GuiCamera> GuiCameraQsp;

};

Q_DECLARE_METATYPE(Isis::GuiCameraQsp);

#endif // GuiCamera_h
