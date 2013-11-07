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
#include "ControlList.h"

#include <QAction>
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFuture>
#include <QInputDialog>
#include <QLabel>
#include <QProgressDialog>
#include <QtConcurrentMap>
#include <QXmlStreamWriter>

#include "Color.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Project.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * Create an control list from a control list name and path (does not read Controls).
   *
   * @param name The ControlList's name (i.e. import1, import2, ...)
   * @param path The ControlList's folder name (i.e. import1, import2, ...)
   * @param parent The Qt-relationship parent
   */
  ControlList::ControlList(QString name, QString path, QObject *parent) : QObject(parent) {
    m_name = name;
    m_path = path;
  }


  /**
   * Create a blank control list.
   *
   * @param parent The Qt-relationship parent
   */
  ControlList::ControlList(QObject *parent) : QObject(parent) {
  }


  /**
   * Create an control list from a list of controls
   *
   * @param controls The list of controls
   * @param parent The Qt-relationship parent
   */
  ControlList::ControlList(QList<Control *> controls, QObject *parent) : QObject(parent) {
    append(controls);
  }


  /**
   * Create an control list from XML
   *
   * @param project The project with the control list
   * @param xmlReader The XML reader currently at an <controlList /> tag.
   * @param parent The Qt-relationship parent
   */
  ControlList::ControlList(Project *project, XmlStackedHandlerReader *xmlReader, QObject *parent) :
      QObject(parent) {
    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  /**
   * Copy constructor.
   *
   * @param other The ControlList to copy
   */
  ControlList::ControlList(const ControlList &other) :
      QList<Control *>(other) {
    m_name = other.m_name;
    m_path = other.m_path;
  }


  /**
   * Create an control list from a list of control net file names. This is slow (serial) and not 
   * recommended.
   */
  ControlList::ControlList(QStringList &fileNames) {
    foreach (QString fileName, fileNames) {
      try {
        Control *control = new Control(fileName);
        append(control);
      }
      catch (IException &) {
      }
    }
  }


  /**
   * Destructor. This does not free the Controls from memory.
   */
  ControlList::~ControlList() {
    emit deletingList(this);
  }


  /**
   * @see QList<Control *>::append()
   */
  void ControlList::append(Control * const & value) {
    QList<Control *>::append(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::append()
   */
  void ControlList::append(const QList<Control *> &value) {
    QList<Control *>::append(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::clear()
   */
  void ControlList::clear() {
    bool countChanging = count();
    QList<Control *>::clear();
    if (countChanging) {
      emit countChanged(count());
    }
  }


  /**
   * @see QList<Control *>::erase()
   */
  QList<Control *>::iterator ControlList::erase(iterator pos) {
    iterator result = QList<Control *>::erase(pos);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Control *>::erase()
   */
  QList<Control *>::iterator ControlList::erase(iterator begin, iterator end) {
    iterator result = QList<Control *>::erase(begin, end);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Control *>::insert()
   */
  void ControlList::insert(int i, Control * const & value) {
    QList<Control *>::insert(i, value);

    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::insert()
   */
  QList<Control *>::iterator ControlList::insert(iterator before, Control * const & value) {
    iterator result = QList<Control *>::insert(before, value);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Control *>::prepend()
   */
  void ControlList::prepend(Control * const & value) {
    QList<Control *>::prepend(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::push_back()
   */
  void ControlList::push_back(Control * const & value) {
    QList<Control *>::push_back(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::push_front()
   */
  void ControlList::push_front(Control * const & value) {
    QList<Control *>::push_front(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::removeAll()
   */
  int ControlList::removeAll(Control * const & value) {
    int result = QList<Control *>::removeAll(value);

    if (result != 0) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * @see QList<Control *>::removeAt()
   */
  void ControlList::removeAt(int i) {
    QList<Control *>::removeAt(i);
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::removeFirst()
   */
  void ControlList::removeFirst() {
    QList<Control *>::removeFirst();
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::removeLast()
   */
  void ControlList::removeLast() {
    QList<Control *>::removeLast();
    emit countChanged(count());
  }


  /**
   * @see QList<Control *>::removeOne()
   */
  bool ControlList::removeOne(Control * const & value) {
    bool result = QList<Control *>::removeOne(value);

    if (result) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * @see QList<Control *>::swap()
   */
  void ControlList::swap(QList<Control *> &other) {
    QList<Control *>::swap(other);

    if (count() != other.count()) {
      emit countChanged(count());
    }
  }


  /**
   * @see QList<Control *>::takeAt()
   */
  Control *ControlList::takeAt(int i) {
    Control * result = QList<Control *>::takeAt(i);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Control *>::takeFirst()
   */
  Control *ControlList::takeFirst() {
    Control *result = QList<Control *>::takeFirst();
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Control *>::takeLast()
   */
  Control *ControlList::takeLast() {
    Control *result = QList<Control *>::takeLast();
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Control *>::operator+=()
   */
  ControlList &ControlList::operator+=(const QList<Control *> &other) {
    QList<Control *>::operator+=(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * @see QList<Control *>::operator+=()
   */
  ControlList &ControlList::operator+=(Control * const &other) {
    QList<Control *>::operator+=(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * @see QList<Control *>::operator<<()
   */
  ControlList &ControlList::operator<<(const QList<Control *> &other) {
    QList<Control *>::operator<<(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * @see QList<Control *>::operator<<()
   */
  ControlList &ControlList::operator<<(Control * const &other) {
    QList<Control *>::operator<<(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * @see QList<Control *>::operator=()
   */
  ControlList &ControlList::operator=(const QList<Control *> &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<Control *>::operator=(rhs);

    if (countChanging) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Assignment operator
   *
   * @param rhs The right hand side of the '=' operator
   * @return *this
   */
  ControlList &ControlList::operator=(const ControlList &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<Control *>::operator=(rhs);

    m_name = rhs.m_name;
    m_path = rhs.m_path;

    if (countChanging) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Set the human-readable name of this control list. This is really only useful for project
   *   control lists (not anonymous temporary ones).
   *
   * @param newName The name to give this control list
   */
  void ControlList::setName(QString newName) {
    m_name = newName;
  }


  /**
   * Set the relative path (from the project root) to this control list's folder. This is really
   *   only useful for project control lists (not anonymous temporary ones).
   *
   * @param newPath The path to the controls in this control list
   */
  void ControlList::setPath(QString newPath) {
    m_path = newPath;
  }


  /**
   * Get the human-readable name of this control list
   *
   * @return The name of the control list (or an empty string if anonymous).
   */
  QString ControlList::name() const {
    return m_name;
  }


  /**
   * Get the path to these controls in the control list (relative to project root). This only
   *   applies to a control list from the project.
   *
   * @return The path to the controls in the control list (or an empty string if unknown).
   */
  QString ControlList::path() const {
    return m_path;
  }


  /**
   * Delete all of the contained Controls from disk (see Control::deleteFromDisk())
   */
  void ControlList::deleteFromDisk(Project *project) {
    foreach (Control *control, *this) {
      control->deleteFromDisk();
    }

    if (!m_path.isEmpty()) {
      QFile::remove(project->cnetRoot() + "/" + m_path + "/controlNetworks.xml");

      QDir dir;
      dir.rmdir(project->cnetRoot() + "/" + m_path);
    }
  }


  /**
   * Convert this control list into XML format for saving/restoring capabilities.
   *
   * This writes:
   * <pre>
   *   <controlList name="..." path="..."/>
   * </pre>
   * to the given xml stream, and creates an 'controls.xml' inside the folder with the controls.
   * Inside the controlNetworks.xml, this writes:
   *
   * <pre>
   *   <controls>
   *     ...
   *   </controls>
   * </pre>
   */
  void ControlList::save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot)
      const {
    stream.writeStartElement("controlList");
    stream.writeAttribute("name", m_name);
    stream.writeAttribute("path", m_path);

    FileName settingsFileName(
        Project::cnetRoot(newProjectRoot.toString()) + "/" + m_path +
                                 "/controlNetworks.xml");

    if (!settingsFileName.dir().mkpath(settingsFileName.path())) {
      throw IException(IException::Io,
                       QString("Failed to create directory [%1]")
                         .arg(settingsFileName.path()),
                       _FILEINFO_);
    }

    QFile controlListContentsFile(settingsFileName.toString());

    if (!controlListContentsFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
      throw IException(IException::Io,
          QString("Unable to save control information for [%1] because [%2] could not be opened "
                  "for writing")
            .arg(m_name).arg(settingsFileName.original()),
          _FILEINFO_);
    }

    QXmlStreamWriter controlDetailsWriter(&controlListContentsFile);
    controlDetailsWriter.setAutoFormatting(true);
    controlDetailsWriter.writeStartDocument();

    int countWidth = QString("%1L").arg(count()).size() - 1;
    QChar paddingChar('0');

    QLabel *progressLabel = new QLabel;

    QProgressDialog progressDialog;
    progressDialog.setLabel(progressLabel);
    progressDialog.setRange(-1, count());
    progressDialog.setValue(-1);

    controlDetailsWriter.writeStartElement("controls");

    QFuture<void *> future = QtConcurrent::mapped(*this,
                                                  CopyControlDataFunctor(project, newProjectRoot));
    for (int i = 0; i < count(); i++) {
      int newProgressValue = progressDialog.value() + 1;
      progressLabel->setText(
          tr("Saving Control Information for [%1] - %L2/%L3 done")
            .arg(m_name)
            .arg(newProgressValue, countWidth, 10, paddingChar)
            .arg(count()));
      progressDialog.setValue(newProgressValue);
      future.resultAt(i);
    }

    progressLabel->setText(tr("Finalizing..."));
    progressDialog.setRange(0, 0);
    progressDialog.setValue(0);

    foreach (Control *control, *this) {
      control->save(controlDetailsWriter, project, newProjectRoot);
    }

    controlDetailsWriter.writeEndElement();

    controlDetailsWriter.writeEndDocument();

    stream.writeEndElement();
  }


  ControlList::CopyControlDataFunctor::CopyControlDataFunctor(const Project *project,
                                                              FileName newProjectRoot) {
    m_project = project;
    m_newProjectRoot = newProjectRoot;
  }


  ControlList::CopyControlDataFunctor::CopyControlDataFunctor(const CopyControlDataFunctor &other) {
    m_project = other.m_project;
    m_newProjectRoot = other.m_newProjectRoot;
  }


  ControlList::CopyControlDataFunctor::~CopyControlDataFunctor() {
  }


  void *ControlList::CopyControlDataFunctor::operator()(Control * const &controlToCopy) {
    controlToCopy->copyToNewProjectRoot(m_project, m_newProjectRoot);
    return NULL;
  }


  ControlList::CopyControlDataFunctor &ControlList::CopyControlDataFunctor::operator=(
      const CopyControlDataFunctor &rhs) {
    m_project = rhs.m_project;
    m_newProjectRoot = rhs.m_newProjectRoot;
    return *this;
  }


  /**
   * Create an XML Handler (reader) that can populate the ControlList class data. See
   *   ControlList::save() for the expected format.
   *
   * @param controlList The control list we're going to be initializing
   * @param project The project that contains the control list
   */
  ControlList::XmlHandler::XmlHandler(ControlList *controlList, Project *project) {
    m_controlList = controlList;
    m_project = project;
  }


  /**
   * Handle an XML start element. This expects <controlList/> and <control/> elements (it reads both
   *   the project XML and the controls.xml file).
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool ControlList::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "controlList") {
        QString name = atts.value("name");
        QString path = atts.value("path");

        if (!name.isEmpty()) {
          m_controlList->setName(name);
        }

        if (!path.isEmpty()) {
          m_controlList->setPath(path);
        }
      }
      else if (localName == "controlNet") {
        m_controlList->append(new Control(m_project->cnetRoot() + "/" +
                                          m_controlList->path(), reader()));
      }
    }

    return true;
  }


  /**
   * Handle an XML end element. This handles <controlList /> by opening and reading the controls.xml
   *   file.
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool ControlList::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName) {
    if (localName == "controlList") {
      XmlHandler handler(m_controlList, m_project);

      XmlStackedHandlerReader reader;
      reader.pushContentHandler(&handler);
      reader.setErrorHandler(&handler);

      QString controlListXmlPath = m_project->cnetRoot() + "/" + m_controlList->path() +
                                 "/controlNetworks.xml";
      QFile file(controlListXmlPath);

      if (!file.open(QFile::ReadOnly)) {
        throw IException(IException::Io,
                         QString("Unable to open [%1] with read access")
                           .arg(controlListXmlPath),
                         _FILEINFO_);
      }

      QXmlInputSource xmlInputSource(&file);
      if (!reader.parse(xmlInputSource))
        throw IException(IException::Io,
                         tr("Failed to open control list XML [%1]").arg(controlListXmlPath),
                         _FILEINFO_);
    }

    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }
}
