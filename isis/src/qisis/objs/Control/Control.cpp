#include "Control.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QUuid>
#include <QXmlStreamWriter>

#include "ControlNet.h"
#include "ControlDisplayProperties.h"
#include "FileName.h"
#include "IString.h"
#include "IException.h"
#include "Project.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * Create a Control from control net located on disk.
   *
   * @param cNetFileName The name of the control net on disk
   * @param parent The Qt-relationship parent
   */
  Control::Control(QString cNetFileName, QObject *parent) : QObject(parent) {

    m_fileName = cNetFileName;

    m_controlNet = NULL;
    m_displayProperties = NULL;

    openControlNet();

    m_displayProperties
        = new ControlDisplayProperties(FileName(m_fileName).name(), this);

    m_id = new QUuid(QUuid::createUuid());
  }


  /**
   * Create a Control from a control network that has already been created and read from disk.
   *  
   * @param controlNet The actual control network
   * @param cnetFileName The name of the control net on disk
   * @param parent The Qt-relationship parent
   */
  Control::Control(ControlNet *controlNet, QString cnetFileName, QObject *parent) :
      QObject(parent) {

    m_fileName = cnetFileName;

    m_controlNet = controlNet;
    m_displayProperties = NULL;

    m_displayProperties
        = new ControlDisplayProperties(FileName(m_fileName).name(), this);

    m_id = new QUuid(QUuid::createUuid());
  }


  /**
   * Construct this control from XML.  TODO:  Is this for reading saved project?
   *  
   * @param cnetFolder Location of control xml - /work/.../projectRoot/cnets/controlNetwork1 
   * @param xmlReader An XML reader that's up to an <control/> tag.  
   * @param parent The Qt-relationship parent
   */
  Control::Control(FileName cnetFolder, XmlStackedHandlerReader *xmlReader, QObject *parent) :
      QObject(parent) {
    m_controlNet = NULL;
    m_displayProperties = NULL;
    m_id = NULL;

    xmlReader->pushContentHandler(new XmlHandler(this, cnetFolder));
  }


  Control::~Control() {
    delete m_controlNet;
    m_controlNet = NULL;

    delete m_id;
    m_id = NULL;

    //  Image is a "Qt" parent of display properties, so the Image QObject
    //    destructor will take care of deleting the display props. See call to
    //    DisplayProperties' constructor.
    m_displayProperties = NULL;
  }


  ControlNet *Control::controlNet() {
    if (!m_controlNet) {
      openControlNet();
    }

    return m_controlNet;
  }
  /**
   * Get the ControlNet * associated with this display property. This will
   *   allocate the ControlNet * if one is not already present.
   */
  void Control::openControlNet() {
    if(!m_controlNet) {
      try {
        m_controlNet = new ControlNet(m_fileName);
      }
      catch(IException &e) {
        throw IException(e, IException::Programmer, "Error opening control net.", _FILEINFO_);
      }
    }
  }


  /**
   * Cleans up the ControlNet *. You want to call this once you're sure you are
   *   done with the ControlNet because the OS will limit how many of these we
    *  have open.
   */  
  void Control::closeControlNet() {
    if (m_controlNet) {
      delete m_controlNet;
      m_controlNet = NULL;
    }
  }


  ControlDisplayProperties *Control::displayProperties() {
    return m_displayProperties;
  }


  const ControlDisplayProperties *Control::displayProperties() const {

    return m_displayProperties;

  }


  QString Control::fileName() const {
    return m_fileName;
  }


  QString Control::id() const {
    return m_id->toString().remove(QRegExp("[{}]"));
  }


  void Control::copyToNewProjectRoot(const Project *project, FileName newProjectRoot) {
    if (FileName(newProjectRoot) != FileName(project->projectRoot())) {

      FileName newCnetFileName(project->cnetRoot(newProjectRoot.toString()) + "/" +
          FileName(m_fileName).dir().dirName() + "/" + FileName(m_fileName).name());
      controlNet()->Write(newCnetFileName.toString());
    }

  }


  /**
   * Delete the control net from disk. The control net will no longer be accessible until you call
   *   updateFileName().
   */
  void Control::deleteFromDisk() {

    if (!QFile::remove(m_fileName)) {
      throw IException(IException::Io,
                       tr("Could not remove file [%1]").arg(m_fileName),
                       _FILEINFO_);
    }

    // If we're the last thing in the folder, remove the folder too.
    QDir dir;
    dir.rmdir(FileName(m_fileName).path());
  }


  /**
   * Change the on-disk file name for this control to be where the control ought to be in the given
   *   project.
   *
   * @param project The project that this control is stored in
   */
  void Control::updateFileName(Project *project) {
    closeControlNet();

    FileName original(m_fileName);
    FileName newName(project->cnetRoot() + "/" +
                     original.dir().dirName() + "/" + original.name());
    m_fileName = newName.expanded();
  }


  void Control::save(QXmlStreamWriter &stream, const Project *project,
                     FileName newProjectRoot) const {
    stream.writeStartElement("controlNet");
    stream.writeAttribute("id", m_id->toString());
    //  Change filename to new path
    stream.writeAttribute("fileName", FileName(m_fileName).name());

    m_displayProperties->save(stream, project, newProjectRoot);

    stream.writeEndElement();
  }


  Control::XmlHandler::XmlHandler(Control *control, FileName cnetFolder) {
    m_control = control;
    m_cnetFolder = cnetFolder;
  }


  bool Control::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                         const QString &qName, const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "controlNet") {
        QString id = atts.value("id");
        QString path = atts.value("path");
        QString fileName = atts.value("fileName");

        if (!id.isEmpty()) {
          delete m_control->m_id;
          m_control->m_id = NULL;
          m_control->m_id = new QUuid(id.toAscii());
        }

        if (!fileName.isEmpty()) {
          m_control->m_fileName = m_cnetFolder.expanded() + "/" + fileName;
          m_control->openControlNet();
        }
      }
      else if (localName == "displayProperties") {
        m_control->m_displayProperties = new ControlDisplayProperties(reader());
      }
    }

    return true;
  }
}



