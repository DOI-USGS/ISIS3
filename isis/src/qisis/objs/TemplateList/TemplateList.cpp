/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "TemplateList.h"
#include <QDir>

#include "FileName.h"
#include "IException.h"
#include "Project.h"

namespace Isis {
  /**
   * Create a template from a file name, type, and path.
   *
   * @param name The TemplateList's name (i.e. import1, import2, ...)
   * @param type The TemplateList's folder name (i.e. maps, registrations, ...)
   * @param path Path to the TemplateList's folder from the template root
   * @param parent The Qt-relationship parent
   */
  TemplateList::TemplateList(QString name, QString type, QString path, QObject *parent) : QObject(parent){
    m_name = name;
    m_path = path;
    m_type = type;
  }


  /**
   * Creates a blank template list.
   *
   * @param parent The Qt-relationship parent.
   */
  TemplateList::TemplateList(QObject *parent) : QObject(parent) {
  }


  /**
   * Copy constructor.
   *
   * @param other The TemplateList to copy
   */
  TemplateList::TemplateList(const TemplateList &other) :
      QList<Template *>(other) {
    m_path = other.m_path;
    m_name = other.m_name;
    m_type = other.m_type;
  }


  /**
   * Destructor.
   */
  TemplateList::~TemplateList() {
    // At the moment, this only occurs during an ImportTemplateWorkOrder undo, where it is handled
  }


  /**
   * Get the human-readable name of this TemplateList
   *
   * @return @b QString The name of the TemplateList.
   */
  QString TemplateList::name() const{
    return m_name;
  }


  /**
   * Get the type of template in this TemplateList
   *
   * @return @b QString The type of template found in this TemplateList.
   */
  QString TemplateList::type() const{
    return m_type;
  }


  /**
   * Get the path to these Templates in the TemplateList (relative to project root).
   *
   * @return @b QString The path to the Templates in the TemplateList.
   */
  QString TemplateList::path() const{
    return m_path;
  }


  /**
   * Set the human-readable name of this TemplateList.
   *
   * @param newName The name to give this TemplateList
   */
  void TemplateList::setName(QString newName) {
    m_name = newName;
  }


  /**
   * Set the type of template for of this TemplateList.
   *
   * @param newType The type to give this TemplateList
   */
  void TemplateList::setType(QString newType) {
    m_type = newType;
  }


  /**
   * Set the relative path (from the project root) to this TemplateList's folder.
   *
   * @param newPath The path to the templates in this TemplateList
   */
  void TemplateList::setPath(QString newPath) {
    m_path = newPath;
  }


  /**
   * Delete all of the contained Templates from disk
   *
   * @param project Project to delete templates from
   *
   * @see Template::deleteFromDisk()
   */
  void TemplateList::deleteFromDisk(Project *project) {
    foreach (Template *currentTemplate, *this) {
      currentTemplate->deleteFromDisk();
    }

    if (!m_path.isEmpty()) {
      QFile::remove(project->templateRoot() + "/" + m_path + "/templates.xml");

      QDir dir;
      dir.rmdir(project->templateRoot() + "/" + m_path);
    }
  }


  /**
   * Convert this TemplateList into XML format for saving/restoring capabilities.
   *
   * This writes:
   * <pre>
   *   <templateList name="..." type= "..." path="..."/>
   * </pre>
   * to the given xml stream, and creates an 'templates.xml' inside the folder with the templates.
   * Inside the templates.xml, this writes:
   *
   * <pre>
   *   <templates>
   *     ...
   *   </templates>
   * </pre>
   *
   * @param stream XML stream that contains the TemplateList data
   * @param project Project to save TemplateList from
   * @param newProjectRoot Filename root to save TemplateList to
   *
   * @throws IException::Io "Unable Failed to create directory"
   * @throws IException::Io "Unable to save template information, could not be opened for writing"
   */
  void TemplateList::save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot)
      const {

    if (m_type == "maps") {
      stream.writeStartElement("mapTemplateList");
    }
    else if (m_type == "registrations") {
      stream.writeStartElement("regTemplateList");
    }
    else {
      throw IException(IException::Io,
                       QString("Attempting to save unsupported template file type: [%1]").arg(m_type),
                       _FILEINFO_);
    }
    stream.writeAttribute("name", m_name);
    stream.writeAttribute("type", m_type);
    stream.writeAttribute("path", m_path);

    FileName settingsFileName(Project::templateRoot(newProjectRoot.toString())
                              + "/" + m_type + "/" + m_name + "/templates.xml");

    if (!settingsFileName.dir().mkpath(settingsFileName.path())) {
      throw IException(IException::Io,
                       QString("Failed to create directory [%1]").arg(settingsFileName.path()),
                       _FILEINFO_);
    }

    QFile templateListContentsFile(settingsFileName.toString());

    if (!templateListContentsFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
      throw IException(IException::Io,
          QString("Unable to save template information for [%1] because [%2] could not be opened "
                  "for writing")
            .arg(m_name).arg(settingsFileName.original()),
          _FILEINFO_);
    }

    QXmlStreamWriter templateDetailsWriter(&templateListContentsFile);
    templateDetailsWriter.setAutoFormatting(true);
    templateDetailsWriter.writeStartDocument();
    templateDetailsWriter.writeStartElement("templates");

    foreach (Template *currentTemplate, *this) {
      currentTemplate->save(templateDetailsWriter, project, newProjectRoot);

      QString newPath = newProjectRoot.toString() + "/templates/" + m_type + "/" + m_name;

      if (currentTemplate->fileName() !=
          newPath + "/" + FileName(currentTemplate->fileName()).name()) {
        QFile::copy(currentTemplate->fileName(),
                    newPath + "/" + FileName(currentTemplate->fileName()).name() );
      }
    }

    templateDetailsWriter.writeEndElement();
    templateDetailsWriter.writeEndDocument();

    stream.writeEndElement();
  }


  /**
   * Create an XML Handler (reader/writer) that can populate the TemplateList class data. See
   * TemplateList::save() for the expected format.
   *
   * @param templateList The template list we're going to be initializing
   * @param project The project that contains the TemplateList
   */
  TemplateList::XmlHandler::XmlHandler(TemplateList *templateList, Project *project) {
    m_templateList = templateList;
    m_project = project;
  }


  /**
   * Handle an XML start element. This expects <templateList/> and <template/> elements (it reads both
   * the project XML and the templates.xml file).
   *
   * @param namespaceURI ???
   * @param localName The keyword name given to the member variable in the XML
   * @param qName ???
   * @param atts The attribute containing the keyword value given for the given localName
   *
   * @return @b bool If we should continue reading the XML (usually true).
   */
  bool TemplateList::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "mapTemplateList" || localName == "regTemplateList") {
        QString name = atts.value("name");
        QString type = atts.value("type");
        QString path = atts.value("path");

        if (!name.isEmpty()) {
          m_templateList->setName(name);
        }

        if (!type.isEmpty()) {
          m_templateList->setType(type);
        }

        if (!path.isEmpty()) {
          m_templateList->setPath(path);
        }
      }
      else if (localName == "template") {
        m_templateList->append(new Template(m_project->templateRoot(), "", ""));
      }
    }

    return true;
  }


  /**
   * Handle an XML end element. This handles <templateList /> by opening and reading the templates.xml
   * file.
   *
   * @param namespaceURI ???
   * @param localName The keyword name given to the member variable in the XML
   * @param qName ???
   *
   * @return @b bool If we should continue reading the XML (usually true).
   *
   * @throws IException::Io "Unable to open with read access"
   * @throws IException::Io "Failed to open TemplateList XML"
   */
  bool TemplateList::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName) {
    if (localName == "mapTemplateList" || localName == "regTemplateList") {

      QString templateListXmlPath = m_project->templateRoot() + "/" + m_templateList->type() + "/"
                                    + m_templateList->name() + "/templates.xml";
      templateListXmlPath = QDir::cleanPath(templateListXmlPath);

      QFile file(templateListXmlPath);

      if (!file.open(QFile::ReadOnly)) {
        throw IException(IException::Io,
                         QString("Unable to open [%1] with read access")
                           .arg(templateListXmlPath),
                         _FILEINFO_);
      }

      QXmlStreamReader reader(&file);
      while (!reader.atEnd()) {
        reader.readNext();
      }
      if (reader.hasError()) {       
        throw IException(IException::Io,
                         tr("Failed to open TemplateList XML [%1]").arg(templateListXmlPath),
                         _FILEINFO_);
      }
    }

    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }
}
