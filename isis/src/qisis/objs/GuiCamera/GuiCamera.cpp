#include "GuiCamera.h"

#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QUuid>
#include <QXmlStreamWriter>

#include <hdf5.h>
#include <hdf5_hl.h> // in the hdf5 library


#include "Camera.h"
#include "GuiCameraDisplayProperties.h"
#include "IString.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"

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

    m_spacecraftNameShort = camera->spacecraftNameShort();
    m_spacecraftNameLong = camera->spacecraftNameLong();
    m_instrumentNameShort = camera->instrumentNameShort();
    m_instrumentNameLong = camera->instrumentNameLong();

    QString displayStr = m_spacecraftNameShort + "/" + m_instrumentNameShort;

    m_displayProperties = new GuiCameraDisplayProperties(displayStr, this);

    m_id = new QUuid(QUuid::createUuid());
  }



//  GuiCamera::GuiCamera(Project *project, XmlStackedHandlerReader *xmlReader,
//                         QObject *parent) : QObject(parent) {
// TODO: does xml stuff need project???
//    m_id = NULL;
//    xmlReader->pushContentHandler(new XmlHandler(this, project));
//    xmlReader->setErrorHandler(new XmlHandler(this, project));
//  }



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


//  PvlObject GuiCamera::pvlObject(QString resultsName, QString settingsName,
//                                     QString statisticsName) {

//    PvlObject pvl(resultsName);
//    pvl += PvlKeyword("RunTime", runTime());
//    if (m_controlNetworkFileName->expanded() != "") {
//      pvl += PvlKeyword("OutputControlNetwork", controlNetworkFileName());
//    }
//    pvl += bundleSettings()->pvlObject(settingsName);
//    pvl += bundleResults()->pvlObject(statisticsName);
//    return pvl;

//  }


  /**
   * This returns the NAIF body code of the target
   *
   * @return @b SpiceInt NAIF body code
   *
   */
//  SpiceInt GuiCamera::naifBodyCode() const {
//    return *m_bodyCode;
//  }


  /**
   * returns "a" radius
   *
   * @return Distance value of body radius "a"
   */
//  Distance GuiCamera::radiusA() const {
//    return m_radii[0];
//  }


  /**
   * returns "a" radius sigma
   *
   * @return Distance value of body radius "a" sigma
   */
//  Distance GuiCamera::sigmaRadiusA() const {
//    return m_sigmaRadii[0];
//  }

  /**
   * returns "b" radius
   *
   * @return Distance value of body radius "b"
   */
//  Distance GuiCamera::radiusB() const {
//    return m_radii[1];
//  }


  /**
   * returns "b" radius sigma
   *
   * @return Distance value of body radius "b" sigma
   */
//  Distance GuiCamera::sigmaRadiusB() const {
//    return m_sigmaRadii[1];
//  }


  /**
   * returns "c" radius
   *
   * @return Distance value of body radius "c"
   */
//  Distance GuiCamera::radiusC() const {
//    return m_radii[2];
//  }


  /**
   * returns "c" radius sigma
   *
   * @return Distance value of body radius "c" sigma
   */
//  Distance GuiCamera::sigmaRadiusC() const {
//    return m_sigmaRadii[2];
//  }


  /**
   * returns mean radius
   *
   * @return Distance value of body mean radius
   */
//  Distance GuiCamera::meanRadius() const {
//    Distance meanRadius = m_radii[0] + m_radii[1] + m_radii[2];

//    meanRadius = meanRadius/3.0;

//    return meanRadius;
//  }


  /**
   * returns mean radius sigma
   *
   * @return Distance value of body mean radius
   */
//  Distance GuiCamera::sigmaMeanRadius() const {
//    Distance sigmaMeanRadius = m_sigmaRadii[0] + m_sigmaRadii[1] + m_sigmaRadii[2];

//    sigmaMeanRadius = sigmaMeanRadius/3.0;

//    return sigmaMeanRadius;
//  }


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


//  QDataStream &GuiCamera::write(QDataStream &stream) const {
//    stream << m_id->toString()
//           << m_runTime;

//    return stream;
//  }



//  QDataStream &GuiCamera::read(QDataStream &stream) {

//    QString id;
//    stream >> id;
//    delete m_id;
//    m_id = NULL;
//    m_id = new QUuid(id);

//    stream >> m_runTime;

//    return stream;
//  }



//  QDataStream &operator<<(QDataStream &stream, const GuiCamera &GuiCamera) {
//    return GuiCamera.write(stream);
//  }



//  QDataStream &operator>>(QDataStream &stream, GuiCamera &GuiCamera) {
//    return GuiCamera.read(stream);
//  }



//  void GuiCamera::savehdf5(FileName outputfilename) const {
//    const H5std_string  hdfFileName(outputfilename.expanded().toStdString());
//Is this the right way to have a dynamic file name?  What about PATH?
//    // Try block to detect exceptions raised by any of the calls inside it
//    try {
//      /*
//       * Turn off the auto-printing when failure occurs so that we can
//       * handle the errors appropriately
//       */
//      H5::Exception::dontPrint();
//      /*
//       * Create a new file using H5F_ACC_TRUNC access,
//       * default file creation properties, and default file
//       * access properties.
//       */
//      H5::H5File hdfFile = H5::H5File( hdfFileName, H5F_ACC_EXCL );
//      hid_t fileId = hdfFile.getId();

//      QString objectName = "/GuiCamera";
//      H5LTset_attribute_string(fileId, objectName.toLatin1(), "runTime", m_runTime.toAscii());
//      H5LTset_attribute_string(fileId, objectName.toLatin1(), "controlNetworkFileName",
//                               m_controlNetworkFileName->expanded().toLatin1());

//      //??? H5::Group settingsGroup = H5::Group(hdfFile.createGroup("/GuiCamera/BundleSettings"));
//        ???
//      //???H5::Group settingsGroup = hdfFile.createGroup("/GuiCamera/BundleSettings");
//      QString groupName = objectName + "/BundleSettings";
//      hid_t groupId = H5Gcreate(fileId, groupName.toLatin1(), H5P_DEFAULT, H5P_DEFAULT,
//      H5P_DEFAULT);
//      m_settings->savehdf5(groupId, groupName.toLatin1());
//      groupName = objectName + "/BundleResults";
//      H5::Group resultsGroup  = H5::Group(hdfFile.createGroup(groupName.toLatin1()));
//      m_statisticsResults->savehdf5(fileId, resultsGroup);
      
//    }
//    catch (H5::FileIException error) {
//      QString msg = QString(error.getCDetailMsg());
//      IException hpfError(IException::Unknown, msg, _FILEINFO_);
//      msg = "Unable to save GuiCamera to hpf5 file. "
//            "H5 exception handler has detected a file error.";
//      throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
//    }
//  }
}
