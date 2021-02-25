/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ShapeList.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFuture>
#include <QInputDialog>
#include <QLabel>
#include <QProgressDialog>
#include <QtConcurrentMap>
#include <QXmlStreamWriter>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Project.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * Creates an shape list from an shape list name and path (does not read Shapes).
   *
   * @param name The ShapeList's name (i.e. import1, import2, ...).
   * @param path The ShapeList's folder name (i.e. import1, import2, ...).
   * @param parent The Qt-relationship parent.
   */
  ShapeList::ShapeList(QString name, QString path, QObject *parent) : QObject(parent) {
    m_name = name;
    m_path = path;
  }


  /**
   * Creates a blank shape list.
   *
   * @param parent The Qt-relationship parent.
   */
  ShapeList::ShapeList(QObject *parent) : QObject(parent) {
  }


  /**
   * Creates an shape list from a list of shapes.
   *
   * @param shapes The list of shapes.
   * @param parent The Qt-relationship parent.
   */
  ShapeList::ShapeList(QList<Shape *> shapes, QObject *parent) : QObject(parent) {
    append(shapes);
  }


  /**
   * Creates an shape list from XML.
   *
   * @param project The project with the shape list.
   * @param xmlReader The XML reader currently at an <shapeList /> tag.
   * @param parent The Qt-relationship parent.
   */
  ShapeList::ShapeList(Project *project, XmlStackedHandlerReader *xmlReader, QObject *parent) :
      QObject(parent) {
    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  /**
   * Copy constructor.
   *
   * @param other The shape list to copy.
   */
  ShapeList::ShapeList(const ShapeList &other) :
      QList<Shape *>(other) {
    m_name = other.m_name;
    m_path = other.m_path;
  }


  /**
   * Creates an shape list from a list of cube file names. This is slow (serial) and not recommended.
   *
   * @param fileNames The list of cube fileNames.
   */
  ShapeList::ShapeList(QStringList &fileNames) {
    foreach (QString fileName, fileNames) {
      try {
        Shape *shape = new Shape(fileName);
        append(shape);
      }
      catch (IException &) {
      }
    }
  }


  /**
   * Destructor. This does not free the Shapes from memory.
   */
  ShapeList::~ShapeList() {
  }


  /**
   * Creates a SerialNumberList from the shape list.
   *
   * @return @b SerialNumberList The list of serial numbers for the cubes in the ShapeList.
   */
  SerialNumberList ShapeList::serialNumberList() {

    SerialNumberList result;

    for (int i = 0; i < count(); i++) {
      result.add((*this)[i]->fileName());
    }
    return result;
  }


  /**
   * Appends an shape to the shape list.
   *
   * @param value The shape to be appended.
   *
   * @see QList<Shape *>::append().
   */
  void ShapeList::append(Shape * const &value) {
    QList<Shape *>::append(value);
    emit countChanged(count());
  }


  /**
   * Appends a list of shapes to the shape list.
   *
   * @param value the list of shapes to be appened.
   *
   * @see QList<Shape *>::append().
   */
  void ShapeList::append(const QList<Shape *> &value) {
    QList<Shape *>::append(value);
    emit countChanged(count());
  }


  /**
   * Clears the shape list.
   *
   * @see QList<Shape *>::clear().
   */
  void ShapeList::clear() {
    bool countChanging = count();
    QList<Shape *>::clear();
    if (countChanging) {
      emit countChanged(count());
    }
  }


  /**
   * Erases a single shape from the shape list.
   *
   * @param pos An iterator pointing to the shape to be erased.
   *
   * @return @b QList::iterator An iterator pointing to the shape after the shape that was removed.
   *
   * @see QList<Shape *>::erase()
   */
  QList<Shape *>::iterator ShapeList::erase(iterator pos) {
    iterator result = QList<Shape *>::erase(pos);
    emit countChanged(count());
    return result;
  }


  /**
   * Erases a range of shapes from the shape list.
   * Erases all shapes from begin up to (but not including) end.
   *
   * @param begin An iterator pointing to the first shape to be erased.
   * @param end An iterator pointing to the shape just after the last shape to be erased.
   *                Will be invalid after the call.
   *
   * @return @b QList::iterator An iterator pointing to the shape end refered to before the call.
   *
   * @see QList<Shape *>::erase()
   */
  QList<Shape *>::iterator ShapeList::erase(iterator begin, iterator end) {
    iterator result = QList<Shape *>::erase(begin, end);
    emit countChanged(count());
    return result;
  }


  /**
   * Inserts an shape into the shape list at an index
   *
   * @param i The index at which to insert the shape.
   * @param value the shape to be inserted.
   *
   * @see QList<Shape *>::insert()
   */
  void ShapeList::insert(int i, Shape * const &value) {
    QList<Shape *>::insert(i, value);

    emit countChanged(count());
  }


  /**
   * Inserts an shape into the shape list after an iterator.
   *
   * @param before An iterator pointing to the shape that value will be inserted after
   * @param value The shape to be inserted.
   *
   * @return @b QList::iterator An iterator pointing to the inserted shape.
   *
   * @see QList<Shape *>::insert()
   */
  QList<Shape *>::iterator ShapeList::insert(iterator before, Shape * const &value) {
    iterator result = QList<Shape *>::insert(before, value);
    emit countChanged(count());
    return result;
  }


  /**
   * Inserts an shape at the beginning of the shape list.
   *
   * @param value The shape to be inserted.
   *
   * @see QList<Shape *>::prepend()
   */
  void ShapeList::prepend(Shape * const &value) {
    QList<Shape *>::prepend(value);
    emit countChanged(count());
  }


  /**
   * Appends an shape to the end of the shape list.
   * Equivalent to append().
   *
   * @param value The shape to be appended.
   *
   * @see QList<Shape *>::push_back()
   */
  void ShapeList::push_back(Shape * const &value) {
    QList<Shape *>::push_back(value);
    emit countChanged(count());
  }


  /**
   * Prepends an shape to the beginning of the shape list.
   * Equivalent to prepend().
   *
   * @param value The shape to be appended.
   *
   * @see QList<Shape *>::push_front()
   */
  void ShapeList::push_front(Shape * const &value) {
    QList<Shape *>::push_front(value);
    emit countChanged(count());
  }


  /**
   * Removes all occurances of an shape.
   *
   * @param value The shape to be removed.
   *
   * @return @b int The number of occurances of the shape.
   *
   * @see QList<Shape *>::removeAll()
   */
  int ShapeList::removeAll(Shape * const &value) {
    int result = QList<Shape *>::removeAll(value);

    if (result != 0) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Removes the shape at an index.
   *
   * @param i The index of the shape to be removed.
   *
   * @see QList<Shape *>::removeAt()
   */
  void ShapeList::removeAt(int i) {
    QList<Shape *>::removeAt(i);
    emit countChanged(count());
  }


  /**
   * Removes the shape at the front of the shape list.
   *
   * @see QList<Shape *>::removeFirst()
   */
  void ShapeList::removeFirst() {
    QList<Shape *>::removeFirst();
    emit countChanged(count());
  }


  /**
   * Removes the shape at the end of the shape list.
   *
   * @see QList<Shape *>::removeLast()
   */
  void ShapeList::removeLast() {
    QList<Shape *>::removeLast();
    emit countChanged(count());
  }


  /**
   * Removes the first occurance of an shape.
   *
   * @param value The shape to be removed.
   *
   * @return @b bool True if successful, otherwise false.
   *
   * @see QList<Shape *>::removeOne()
   */
  bool ShapeList::removeOne(Shape * const &value) {
    bool result = QList<Shape *>::removeOne(value);

    if (result) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Swaps the shape list with another list of shapes.
   *
   * @param other The list of shapes to swapped with.
   *
   * @see QList<Shape *>::swap()
   */
  void ShapeList::swap(QList<Shape *> &other) {
    QList<Shape *>::swap(other);

    if (count() != other.count()) {
      emit countChanged(count());
    }
  }


  /**
   * Removes the shape at an index and returns it.
   *
   * @param i The index of the shape to be removed and returned.
   *
   * @return @b Shape * The removed shape.
   *
   * @see QList<Shape *>::takeAt()
   */
  Shape *ShapeList::takeAt(int i) {
    Shape * result = QList<Shape *>::takeAt(i);
    emit countChanged(count());
    return result;
  }


  /**
   * Removes and returns the first shape.
   *
   * @return @b Shape * The first shape.
   *
   * @see QList<Shape *>::takeFirst()
   */
  Shape *ShapeList::takeFirst() {
    Shape *result = QList<Shape *>::takeFirst();
    emit countChanged(count());
    return result;
  }


  /**
   * Removes and returns the last shape.
   *
   * @return @b Shape * The last shape.
   *
   * @see QList<Shape *>::takeLast()
   */
  Shape *ShapeList::takeLast() {
    Shape *result = QList<Shape *>::takeLast();
    emit countChanged(count());
    return result;
  }


  /**
   * Appends a list of shapes to the end of the shape list.
   *
   * @param other The list of shapes to be appended.
   *
   * @return @b ShapeList & A reference to the shapeList.
   *
   * @see append()
   * @see QList<Shape *>::operator+=()
   */
  ShapeList &ShapeList::operator+=(const QList<Shape *> &other) {
    QList<Shape *>::operator+=(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Appends a single shape to the end of the shape list.
   *
   * @param other The shape to be appended.
   *
   * @return @b ShapeList & A reference to the shapeList.
   *
   * @see append()
   * @see QList<Shape *>::operator+=()
   */
  ShapeList &ShapeList::operator+=(Shape * const &other) {
    QList<Shape *>::operator+=(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Appends a list of shapes to the end of the shape list.
   *
   * @param other The list of shapes to be appended.
   *
   * @return @b ShapeList & A reference to the shapeList.
   *
   * @see append()
   * @see QList<Shape *>::operator<<()
   */
  ShapeList &ShapeList::operator<<(const QList<Shape *> &other) {
    QList<Shape *>::operator<<(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Appends a single shape to the end of the shape list.
   *
   * @param other The shape to be appended.
   *
   * @return @b ShapeList & A reference to the shapeList.
   *
   * @see append()
   * @see QList<Shape *>::operator<<()
   */
  ShapeList &ShapeList::operator<<(Shape * const &other) {
    QList<Shape *>::operator<<(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Assigns another list of shapes to the shape list.
   *
   * @param rhs The list of shapes that shapeList will become a copy of.
   *
   * @return @b ShapeList & A reference to the shapeList.
   *
   * @see QList<Shape *>::operator=()
   */
  ShapeList &ShapeList::operator=(const QList<Shape *> &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<Shape *>::operator=(rhs);

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
   * @return @b ShapeList & This shape list.
   */
  ShapeList &ShapeList::operator=(const ShapeList &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<Shape *>::operator=(rhs);

    m_name = rhs.m_name;
    m_path = rhs.m_path;

    if (countChanging) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Set the human-readable name of this shape list. This is really only useful for project
   * shape lists (not anonymous temporary ones).
   *
   * @param newName The name to give this shape list
   */
  void ShapeList::setName(QString newName) {
    m_name = newName;
  }


  /**
   * Set the relative path (from the project root) to this shape list's folder. This is really only
   * useful for project shape lists (not anonymous temporary ones).
   *
   * @param newPath The path to the shapes in this shape list
   */
  void ShapeList::setPath(QString newPath) {
    m_path = newPath;
  }


  /**
   * Get the human-readable name of this shape list
   *
   * @return @b Qstring The name of the shape list (or an empty string if anonymous).
   */
  QString ShapeList::name() const {
    return m_name;
  }


  /**
   * Get the path to the shapes in the shape list (relative to project root). This only applies to
   * an shape list from the project.
   *
   * @return @b QString The path to the shapes in the shape list (or an empty string if unknown).
   */
  QString ShapeList::path() const {
    return m_path;
  }


  /**
   * Delete all of the contained Shapes from disk.
   *
   * @param project The project the shapes in the shape list belong to.
   *
   * @see Shape::deleteFromDisk()
   */
  void ShapeList::deleteFromDisk(Project *project) {
    foreach (Shape *shape, *this) {
      shape->deleteFromDisk();
    }

    if (!m_path.isEmpty()) {
      QFile::remove(project->shapeDataRoot() + "/" + m_path + "/shapes.xml");

      QDir dir;
      dir.rmdir(project->shapeDataRoot() + "/" + m_path);
    }
  }


  /**
   * Convert this shape list into XML format for saving/restoring capabilities.
   *
   * This writes:
   * <pre>
   *   <shapeList name="..." path="..."/>
   * </pre>
   * to the given xml stream, and creates an 'shapes.xml' inside the folder with the shapes.
   * Inside the shapes.xml, this writes:
   *
   * <pre>
   *   <shapes>
   *     ...
   *   </shapes>
   * </pre>
   *
   * @param stream XmlStream to write out the document.
   * @param project The project the shape list will be saved to.
   * @param newProjectRoot The path to the root directory for the new project.
   *
   * @throws iException::Io "Failed to create directory"
   * @throws iException::Io "Unable to save shape information because new file could not be opened for writing"
   */
  void ShapeList::save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot)
      const {
    stream.writeStartElement("shapeList");
    stream.writeAttribute("name", m_name);
    stream.writeAttribute("path", m_path);

    FileName settingsFileName(
        Project::shapeDataRoot(newProjectRoot.toString()) + "/" + m_path + "/shapes.xml");

    if (!settingsFileName.dir().mkpath(settingsFileName.path())) {
      throw IException(IException::Io,
                       QString("Failed to create directory [%1]")
                         .arg(settingsFileName.path()),
                       _FILEINFO_);
    }

    QFile shapeListContentsFile(settingsFileName.toString());

    if (!shapeListContentsFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
      throw IException(IException::Io,
          QString("Unable to save shape information for [%1] because [%2] could not be opened for "
                  "writing")
            .arg(m_name).arg(settingsFileName.original()),
          _FILEINFO_);
    }

    QXmlStreamWriter shapeDetailsWriter(&shapeListContentsFile);
    shapeDetailsWriter.setAutoFormatting(true);
    shapeDetailsWriter.writeStartDocument();

    int countWidth = QString("%1L").arg(count()).size() - 1;
    QChar paddingChar('0');

    QLabel *progressLabel = new QLabel;

    QProgressDialog progressDialog;
    progressDialog.setLabel(progressLabel);
    progressDialog.setRange(-1, count());
    progressDialog.setValue(-1);

    shapeDetailsWriter.writeStartElement("shapes");
    // Mapped is way faster than hundreds/thousands of run() calls... so use mapped for performance
    QFuture<void *> future = QtConcurrent::mapped(*this,
                                                  CopyShapeDataFunctor(project, newProjectRoot));

    for (int i = 0; i < count(); i++) {
      int newProgressValue = progressDialog.value() + 1;
      progressLabel->setText(
          tr("Saving Shape Information for [%1] - %L2/%L3 done")
            .arg(m_name)
            .arg(newProgressValue, countWidth, 10, paddingChar)
            .arg(count()));
      progressDialog.setValue(newProgressValue);
      future.resultAt(i);
    }

    progressLabel->setText(tr("Finalizing..."));
    progressDialog.setRange(0, 0);
    progressDialog.setValue(0);

    foreach (Shape *shape, *this) {
      shape->save(shapeDetailsWriter, project, newProjectRoot);
    }

    shapeDetailsWriter.writeEndElement();

    shapeDetailsWriter.writeEndDocument();

    stream.writeEndElement();
  }


  /**
   * Constructor for CopyShapeDataFunctor.
   *
   * @param project The project that the shape data will be saved to when the functor is used
   * @param newProjectRoot The path to the project root
   */
  ShapeList::CopyShapeDataFunctor::CopyShapeDataFunctor(const Project *project,
                                                        FileName newProjectRoot) {
    m_project = project;
    m_newProjectRoot = newProjectRoot;
  }


  /**
   * Copy constructor for CopyShapeDataFunctor.
   *
   * @param other The functor to copy from
   */
  ShapeList::CopyShapeDataFunctor::CopyShapeDataFunctor(const CopyShapeDataFunctor &other) {
    m_project = other.m_project;
    m_newProjectRoot = other.m_newProjectRoot;
  }


  /**
   * Destructor for CopyShapeDataFunctor.
   */
  ShapeList::CopyShapeDataFunctor::~CopyShapeDataFunctor() {
  }


  /**
   * Copies the cub/ecub files for an shape into m_project.
   * Used by save to copy the shapeList into a new project.
   *
   * @param shapeToCopy The shape to copy into m_project.
   *
   * @see save
   */
  void *ShapeList::CopyShapeDataFunctor::operator()(Shape * const &shapeToCopy) {
    shapeToCopy->copyToNewProjectRoot(m_project, m_newProjectRoot);
    return NULL;
  }


  /**
   * Assignment operator for CopyShapeDataFunctor.
   *
   * @param rhs The functor to assign from
   *
   * @return @b ShapeList::CopyShapeDataFunctor & A reference to a copy of the functor
   */
  ShapeList::CopyShapeDataFunctor &ShapeList::CopyShapeDataFunctor::operator=(
      const CopyShapeDataFunctor &rhs) {
    m_project = rhs.m_project;
    m_newProjectRoot = rhs.m_newProjectRoot;
    return *this;
  }


  /**
   * Create an XML Handler (reader) that can populate the Shape list class data.
   *
   * @param shapeList The shape list we're going to be initializing
   * @param project The project that contains the shape list
   *
   * @see ShapeList::save()
   */
  ShapeList::XmlHandler::XmlHandler(ShapeList *shapeList, Project *project) {
    m_shapeList = shapeList;
    m_project = project;
  }


  /**
   * Handle an XML start element. This expects <shapeList/> and <shape/> elements (it reads both
   * the project XML and the shapes.xml file).
   *
   * @return @b bool If we should continue reading the XML (usually true).
   */
  bool ShapeList::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "shapeList") {
        QString name = atts.value("name");
        QString path = atts.value("path");

        if (!name.isEmpty()) {
          m_shapeList->setName(name);
        }

        if (!path.isEmpty()) {
          m_shapeList->setPath(path);
        }
      }
      else if (localName == "shape") {
        m_shapeList->append(new Shape(m_project->shapeDataRoot() + "/" + m_shapeList->path(),
                                      reader()));
      }
    }

    return true;
  }


  /**
   * Handle an XML end element. This handles <shapeList /> by opening and reading the shapes.xml
   * file.
   *
   * @return @b bool If we should continue reading the XML (usually true).
   *
   * @throws IException::Io "Unable to open with read access"
   * @throws IException::Io "Failed to open shape list XML"
   */
  bool ShapeList::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                         const QString &qName) {
    if (localName == "shapeList") {
      XmlHandler handler(m_shapeList, m_project);

      XmlStackedHandlerReader reader;
      reader.pushContentHandler(&handler);
      reader.setErrorHandler(&handler);

      QString shapeListXmlPath = m_project->shapeDataRoot() + "/" + m_shapeList->path() +
                                 "/shapes.xml";
      QFile file(shapeListXmlPath);

      if (!file.open(QFile::ReadOnly)) {
        throw IException(IException::Io,
                         QString("Unable to open [%1] with read access")
                           .arg(shapeListXmlPath),
                         _FILEINFO_);
      }

      QXmlInputSource xmlInputSource(&file);
      if (!reader.parse(xmlInputSource))
        throw IException(IException::Io,
                         tr("Failed to open shape list XML [%1]").arg(shapeListXmlPath),
                         _FILEINFO_);
    }

    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }
}
