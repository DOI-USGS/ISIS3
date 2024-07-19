#include "GuiCamera.h"

#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QUuid>
#include <QXmlStreamWriter>

#include "Camera.h"
#include "GuiCameraDisplayProperties.h"
#include "IString.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"

namespace Isis {


/**
   * @brief The constructor for this class.
   * @param camera  A traditional Isis::Camera object which we are wrapping in this class.
   * @param parent  A pointer to the object instantiating this object.
   */
  GuiCamera::GuiCamera(Camera *camera, QObject *parent) : QObject(parent) {
    m_id = NULL;

//    m_camera = camera;

    // TODO - initialize GuiCamera members from target
//    *m_bodyCode = target->naifBodyCode();
//    m_radii[0] = target->radii().at(0);
//    m_radii[1] = target->radii().at(1);
//    m_radii[2] = target->radii().at(2);

    m_instrumentId = camera->instrumentId();

    m_spacecraftNameShort = camera->spacecraftNameShort();
    m_spacecraftNameLong = camera->spacecraftNameLong();
    m_instrumentNameShort = camera->instrumentNameShort();
    m_instrumentNameLong = camera->instrumentNameLong();

    QString displayStr = m_spacecraftNameShort + "/" + m_instrumentNameShort;

    m_displayProperties = new GuiCameraDisplayProperties(displayStr, this);

    m_id = new QUuid(QUuid::createUuid());
  }



//  GuiCamera::GuiCamera(const GuiCamera &src)
//      : m_id(new QUuid(src.m_id->toString())) {
//
//    m_bodyCode = new SpiceInt(*src.m_bodyCode);
//
//    m_radii.resize(3, Distance());
//    m_sigmaRadii.resize(3, Distance(3.0, Distance::Kilometers));
// TODO - radii sigma fudged for now
//
//    for (int i = 0; i < 3; i++) {
//      m_radii[i] = src.m_radii[i];
//      m_sigmaRadii[i] = src.m_sigmaRadii[i];
//    }
//
//    m_displayProperties
//        = new GuiCameraDisplayProperties(*src.m_displayProperties);
//
//    int fred=1;
//    m_bodyCode = src.m_bodyCode;
//  }



  /**
   * @brief The Destructor
   */

  GuiCamera::~GuiCamera() {
    delete m_id;
    m_id = NULL;
  }

  
//  GuiCamera &GuiCamera::operator=(const GuiCamera &src) {
//    if (&src != this) {
//      delete m_id;
//      m_id = NULL;
//      m_id = new QUuid(src.m_id->toString());
//    }
//    return *this;
//  }
//  Camera *GuiCamera::camera() {
//      return m_camera;
//  }


  /**
   * @brief Compares two Target Body objects to see if they are equal
   * @param srcGuiCamera GuiCamera object to compare against
   * @return @b bool Returns True if the objects are equal, False if not.
   */
  bool GuiCamera::operator== (const GuiCamera &srcGuiCamera) const {

    if (m_displayProperties->displayName() != srcGuiCamera.displayProperties()->displayName())
      return false;

    return true;
  }



  /*
  GuiCameraDisplayProperties *GuiCamera::displayProperties() {
    return m_displayProperties;
  }
  */

  /**
   * @brief Retrieves the display properties of the camera.
   * @return GuiCameraDisplayProperties
   */
  const GuiCameraDisplayProperties *GuiCamera::displayProperties() const {
    return m_displayProperties;
  }


  /**
   * @brief Retrieves a unique, identifying string associated with this GuiCamera object.
   * @return @b QString returns m_id
   */
  QString GuiCamera::id() const {
    return m_id->toString().remove(QRegExp("[{}]"));
  }
  
  
  /**
   * @brief Retrieve the InstrumentId as appears in the original cube label.
   * @return @b QString Returns m_instrumentId
   */
  QString GuiCamera::instrumentId() {
    return m_instrumentId;
  }
  

  /**
   * @brief Retrieves an abbreviated version for the name of the instrument.
   * @return @b QString Returns m_instrumentNameShort.
   */
  QString GuiCamera::instrumentNameShort() {
    return m_instrumentNameShort;
  }


  /**
   * @brief Retrieves a long version for the name of the instrument.
   * @return @b QString Returns m_instrumentNameLong.
   */
  QString GuiCamera::instrumentNameLong() {
    return m_instrumentNameLong;
  }


  /**
   * @brief Retrieves an abbbreviated name for the spacecraft.
   * @return @b QString Returns m_spacecraftNameShort.
   */
  QString GuiCamera::spacecraftNameShort() {
    return m_spacecraftNameShort;
  }


  /**
   * @brief Retrieves the full name of the spacecraft.
   * @return @b QString Returns m_spacecraftNameLong.
   */
  QString GuiCamera::spacecraftNameLong() {
    return m_spacecraftNameLong;
  }



}
