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
   * Output format:
   *
   *
   * <image id="..." fileName="...">
   *   ...
   * </image>
   *
   * (fileName attribute is just the base name)
   */
//  void GuiCamera::save(QXmlStreamWriter &stream, const Project *project,
//                            FileName newProjectRoot) const {

//    stream.writeStartElement("GuiCamera");
//    // save ID, cnet file name, and run time to stream
//    stream.writeStartElement("generalAttributes");
//    stream.writeTextElement("id", m_id->toString());
//    stream.writeTextElement("runTime", runTime());
//    stream.writeTextElement("fileName", m_controlNetworkFileName->expanded());
//    stream.writeEndElement(); // end general attributes

//    // save settings to stream
//    m_settings->save(stream, project);

//    // save statistics to stream
//    m_statisticsResults->save(stream, project);

//    // save image lists to stream
//    if ( !m_images->isEmpty() ) {
//      stream.writeStartElement("imageLists");

//      for (int i = 0; i < m_images->count(); i++) {
//        m_images->at(i)->save(stream, project, "");
//      }

//      stream.writeEndElement();
//    }
//    stream.writeEndElement(); //end GuiCamera
//  }



//  void GuiCamera::save(QXmlStreamWriter &stream, const Project *project) const {

//    stream.writeStartElement("GuiCamera");

//    // save ID, attributes, and run time to stream
//    stream.writeStartElement("generalAttributes");
//    stream.writeTextElement("id", m_id->toString());
//    stream.writeTextElement("runTime", runTime());
//    stream.writeEndElement(); // end general attributes

//    stream.writeEndElement(); //end GuiCamera
//  }



  /**
   * Create an XML Handler (reader) that can populate the BundleSettings class data. See
   *   BundleSettings::save() for the expected format.
   *
   * @param bundleSettings The image we're going to be initializing
   * @param imageFolder The folder that contains the Cube
   */
//  GuiCamera::XmlHandler::XmlHandler(GuiCamera *GuiCamera, Project *project) {
//    m_xmlHandlerGuiCamera = GuiCamera;
//    m_xmlHandlerProject = NULL;
//    m_xmlHandlerProject = project;
//    m_xmlHandlerCharacters = "";
//  }



//  GuiCamera::XmlHandler::~XmlHandler() {
    // GuiCamera passed in is "this" delete+null will cause problems,no?
//    delete m_xmlHandlerGuiCamera;
//    m_xmlHandlerGuiCamera = NULL;

    // we do not delete this pointer since it was set to a passed in pointer in constructor and we
    // don't own it... is that right???
//    delete m_xmlHandlerProject;
//    m_xmlHandlerProject = NULL;
//  }



  /**
   * Handle an XML start element. This expects <image/> and <displayProperties/> elements.
   *
   * @return If we should continue reading the XML (usually true).
   */
//  bool GuiCamera::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
//                                       const QString &qName, const QXmlAttributes &atts) {
//    m_xmlHandlerCharacters = "";
//
//    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
//
//      if (localName == "GuiCamera") {
//        m_xmlHandlerGuiCamera =
//            BundleSettingsQsp(new GuiCamera(m_xmlHandlerProject, reader()));
//      }
//      else if (localName == "bundleResults") {
//        delete m_xmlHandlerBundleResults;
//        m_xmlHandlerBundleResults = NULL;
//        m_xmlHandlerBundleResults = new BundleResults(m_xmlHandlerProject, reader());
//TODO: need to add constructor for this???
//      }
//      else if (localName == "imageList") {
//        m_xmlHandlerImages->append(new ImageList(m_xmlHandlerProject, reader()));
//      }
//    }
//    return true;
//  }



//  bool GuiCamera::XmlHandler::characters(const QString &ch) {
//    m_xmlHandlerCharacters += ch;
//    return XmlStackedHandler::characters(ch);
//  }



//  bool GuiCamera::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
//                                             const QString &qName) {
//    if (localName == "id") {
//      m_xmlHandlerGuiCamera->m_id = NULL;
//      m_xmlHandlerGuiCamera->m_id = new QUuid(m_xmlHandlerCharacters);
//    }
//    else if (localName == "runTime") {
//      m_xmlHandlerGuiCamera->m_runTime = m_xmlHandlerCharacters;
//    }
//    else if (localName == "fileName") {
//      m_xmlHandlerGuiCamera->m_controlNetworkFileName = NULL;
//      m_xmlHandlerGuiCamera->m_controlNetworkFileName = new FileName(m_xmlHandlerCharacters);
//    }
//    else if (localName == "bundleSettings") {
//      m_xmlHandlerGuiCamera->m_settings =
//          BundleSettingsQsp(new BundleSettings(*m_xmlHandlerBundleSettings));
//    }
//    else if (localName == "bundleResults") {
//      m_xmlHandlerGuiCamera->m_statisticsResults = new BundleResults(*m_xmlHandlerBundleResults);
//    }
//    if (localName == "imageLists") {
//      for (int i = 0; i < m_xmlHandlerImages->size(); i++) {
//        m_xmlHandlerGuiCamera->m_images->append(m_xmlHandlerImages->at(i));
//      }
//      m_xmlHandlerImages->clear();
//    }
//    m_xmlHandlerCharacters = "";
//    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
//  }


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
