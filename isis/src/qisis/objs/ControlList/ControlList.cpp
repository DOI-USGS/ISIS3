/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *
   * @param fileNames Control net file names
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
   * Appends a control pointer to the control list
   *
   * @param value The control pointer to append
   *
   * @see QList<Control *>::append()
   */
  void ControlList::append(Control * const &value) {
    QList<Control *>::append(value);
    emit countChanged(count());
  }


  /**
   * Appends a list of control pointers to the control list
   *
   * @param value The list of control pointers to append
   *
   * @see QList<Control *>::append()
   */
  void ControlList::append(const QList<Control *> &value) {
    QList<Control *>::append(value);
    emit countChanged(count());
  }


  /**
   * Clears the control list
   *
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
   * Erases a control pointer from the control list at the specified position
   *
   * @param pos The position to erase at
   *
   * @return @b QList<Control *>::iterator Iterator to the next item in the list
   *
   * @see QList<Control *>::erase()
   */
  QList<Control *>::iterator ControlList::erase(iterator pos) {
    iterator result = QList<Control *>::erase(pos);
    emit countChanged(count());
    return result;
  }


  /**
   * Erases all control pointers starting at "begin" up to (but not including) end
   *
   * @param begin Iterator to the first control pointer to erase from the list
   * @param end Iterator to the end control pointer (which will not be erased)
   *
   * @return @b QList<Control *>::iterator Iterator to the end control pointer
   *
   * @see QList<Control *>::erase()
   */
  QList<Control *>::iterator ControlList::erase(iterator begin, iterator end) {
    iterator result = QList<Control *>::erase(begin, end);
    emit countChanged(count());
    return result;
  }


  /**
   * Inserts a control pointer at the specified position in the control list
   *
   * @param i The index to insert at
   * @param value The control pointer to insert
   *
   * @see QList<Control *>::insert()
   */
  void ControlList::insert(int i, Control * const &value) {
    QList<Control *>::insert(i, value);

    emit countChanged(count());
  }


  /**
   * Inserts a control pointer before the specified iterator position
   *
   * @param before Iterator to the item to insert the control pointer before
   * @param value The control pointer to insert
   *
   * @return @b QList<Control *>::iterator Iterator to the inserted control pointer
   *
   * @see QList<Control *>::insert()
   */
  QList<Control *>::iterator ControlList::insert(iterator before, Control * const &value) {
    iterator result = QList<Control *>::insert(before, value);
    emit countChanged(count());
    return result;
  }


  /**
   * Prepends a control pointer to the control list
   *
   * @param value The control pointer to prepend to the control list
   *
   * @see QList<Control *>::prepend()
   */
  void ControlList::prepend(Control * const &value) {
    QList<Control *>::prepend(value);
    emit countChanged(count());
  }


  /**
   * Equivalent to append(value)
   *
   * @param value The control pointer to append to the control list
   *
   * @see ControlList::append(Control * const &value)
   * @see QList<Control *>::push_back()
   */
  void ControlList::push_back(Control * const &value) {
    QList<Control *>::push_back(value);
    emit countChanged(count());
  }


  /**
   * Equivalent to prepend(value)
   *
   * @param value The control pointer to prepend to the control list
   *
   * @see ControlList::prepend(Control * const &value)
   * @see QList<Control *>::push_front()
   */
  void ControlList::push_front(Control * const &value) {
    QList<Control *>::push_front(value);
    emit countChanged(count());
  }


  /**
   * Removes all occurences of the control pointer in the control list
   *
   * @param value The control pointer value to remove
   *
   * @return @b int The number of control pointers removed
   *
   * @see QList<Control *>::removeAll()
   */
  int ControlList::removeAll(Control * const &value) {
    int result = QList<Control *>::removeAll(value);

    if (result != 0) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Removes the control pointer at the specified index
   *
   * @param i The index of the control pointer to remove
   *
   * @see QList<Control *>::removeAt()
   */
  void ControlList::removeAt(int i) {
    QList<Control *>::removeAt(i);
    emit countChanged(count());
  }


  /**
   * Removes the first control pointer from the control list
   *
   * @see QList<Control *>::removeFirst()
   */
  void ControlList::removeFirst() {
    QList<Control *>::removeFirst();
    emit countChanged(count());
  }


  /**
   * Removes the last control pointer from the control list
   *
   * @see QList<Control *>::removeLast()
   */
  void ControlList::removeLast() {
    QList<Control *>::removeLast();
    emit countChanged(count());
  }


  /**
   * Removes the first occurence of the control pointer from the control list
   *
   * @param value The control pointer to remove
   *
   * @return @b bool True if a control pointer was removed; otherwise false
   *
   * @see QList<Control *>::removeOne()
   */
  bool ControlList::removeOne(Control * const &value) {
    bool result = QList<Control *>::removeOne(value);

    if (result) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Swaps this control list's control pointers with the other list of control pointers
   *
   * @param other The list of control pointers to swap
   *
   * @see QList<Control *>::swap()
   */
  void ControlList::swap(QList<Control *> &other) {
    QList<Control *>::swap(other);

    if (count() != other.count()) {
      emit countChanged(count());
    }
  }


  /**
   * Remove the control pointer at the specified index and returns it
   *
   * @param i The index of the control pointer to take
   *
   * @return @b Contol * The removed control pointer
   *
   * @see QList<Control *>::takeAt()
   */
  Control *ControlList::takeAt(int i) {
    Control * result = QList<Control *>::takeAt(i);
    emit countChanged(count());
    return result;
  }


  /**
   * Removes the first control pointer from the control list and returns it
   *
   * @return @b Control * The first control pointer in the control list
   *
   * @see QList<Control *>::takeFirst()
   */
  Control *ControlList::takeFirst() {
    Control *result = QList<Control *>::takeFirst();
    emit countChanged(count());
    return result;
  }


  /**
   * Removes the last control pointer from the control list and returns it
   *
   * @return @b Control * The last control pointer in the control list
   *
   * @see QList<Control *>::takeLast()
   */
  Control *ControlList::takeLast() {
    Control *result = QList<Control *>::takeLast();
    emit countChanged(count());
    return result;
  }


  /**
   * Appends control pointers from the other list to this control list
   *
   * @param The list of other control pointers to append
   *
   * @return @b ControlList & Reference to this control list
   *
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
   * Appends a control pointer to this control list
   *
   * @param other The control pointer to append
   *
   * @return @b ControlList & Reference to this control list
   *
   * @see QList<Control *>::operator+=()
   */
  ControlList &ControlList::operator+=(Control * const &other) {
    QList<Control *>::operator+=(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Appends a list of other control pointers to this control list
   *
   * @param other The list of other control pointers to append
   *
   * @return @b ControlList & Reference to this control list
   *
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
   * Appends a control pointer to this control list
   *
   * @param other The control pointer to append
   *
   * @return @b ControlList & Reference to this control list
   *
   * @see QList<Control *>::operator<<()
   */
  ControlList &ControlList::operator<<(Control * const &other) {
    QList<Control *>::operator<<(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Assigns another list of control pointers to this control list
   *
   * @param rhs The other list of control pointers to assign
   *
   * @return @b ControlList & Reference to this control list
   *
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
   *
   * @return @b ControlList & Reference to this ControlList
   *
   * @see ControlList::operator=(const QList<Control *> &rhs)
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
   * control lists (not anonymous temporary ones).
   *
   * @param newName The name to give this control list
   */
  void ControlList::setName(QString newName) {
    m_name = newName;
  }


  /**
   * Set the relative path (from the project root) to this control list's folder. This is really
   * only useful for project control lists (not anonymous temporary ones).
   *
   * @param newPath The path to the controls in this control list
   */
  void ControlList::setPath(QString newPath) {
    m_path = newPath;
  }


  /**
   * Get the human-readable name of this control list
   *
   * @return @b QString The name of the control list (or an empty string if anonymous).
   */
  QString ControlList::name() const {
    return m_name;
  }


  /**
   * Get the path to these controls in the control list (relative to project root). This only
   * applies to a control list from the project.
   *
   * @return @b QString The path to the controls in the control list (or an empty string if unknown).
   */
  QString ControlList::path() const {
    return m_path;
  }


  /**
   * Delete all of the contained Controls from disk
   *
   * @param project Project to delete controls from
   *
   * @see Control::deleteFromDisk()
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
   *
   * @param stream XML stream that contains the control list data
   * @param project Project to save control list from
   * @param newProjectRoot Filename root to save control list to
   *
   * @throws IException::Io "Unable Failed to create directory"
   * @throws IException::Io "Unable to save control information, could not be opened for writing"
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

    // Only copy controls if saving to new location
    if (project->newProjectRoot() != project->projectRoot()) {
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
    }

    foreach (Control *control, *this) {
      control->save(controlDetailsWriter, project, newProjectRoot);
    }

    controlDetailsWriter.writeEndElement();

    controlDetailsWriter.writeEndDocument();

    stream.writeEndElement();
  }


  /**
   * CopyControlDataFunctor constructor.
   *
   * @param project Project to copy from
   * @param newProjectRoot Project to copy to
   */
  ControlList::CopyControlDataFunctor::CopyControlDataFunctor(const Project *project,
                                                              FileName newProjectRoot) {
    m_project = project;
    m_newProjectRoot = newProjectRoot;
  }


  /**
   * CopyControlDataFunctor copy constructor.
   *
   * @param other The other CopyControlDataFunctor to initialize data from
   */
  ControlList::CopyControlDataFunctor::CopyControlDataFunctor(const CopyControlDataFunctor &other) {
    m_project = other.m_project;
    m_newProjectRoot = other.m_newProjectRoot;
  }


  /**
   * CopyControlDataFunctor destructor
   */
  ControlList::CopyControlDataFunctor::~CopyControlDataFunctor() {
  }


  /**
   * Copies the Control from one project to another.
   *
   * @param controlToCopy The Control to copy
   */
  void *ControlList::CopyControlDataFunctor::operator()(Control * const &controlToCopy) {
    controlToCopy->copyToNewProjectRoot(m_project, m_newProjectRoot);
    return NULL;
  }


  /**
   * CopyControlDataFunctor assignment operator.
   *
   * @param rhs The other CopyControlDataFunctor to copy from
   *
   * @return @b ControlList::CopyControlDataFunctor The new copy
   */
  ControlList::CopyControlDataFunctor &ControlList::CopyControlDataFunctor::operator=(
      const CopyControlDataFunctor &rhs) {
    m_project = rhs.m_project;
    m_newProjectRoot = rhs.m_newProjectRoot;
    return *this;
  }


  /**
   * Create an XML Handler (reader/writer) that can populate the ControlList class data. See
   * ControlList::save() for the expected format.
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
   * the project XML and the controls.xml file).
   *
   * @param namespaceURI ???
   * @param localName The keyword name given to the member variable in the XML
   * @param qName ???
   * @param atts The attribute containing the keyword value given for the given localName
   *
   * @return @b bool If we should continue reading the XML (usually true).
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
   * file.
   *
   * @param namespaceURI ???
   * @param localName The keyword name given to the member variable in the XML
   * @param qName ???
   *
   * @return @b bool If we should continue reading the XML (usually true).
   *
   * @throws IException::Io "Unable to open with read access"
   * @throws IException::Io "Failed to open control list XML"
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
