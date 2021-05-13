/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "TargetBodyList.h"

//#include <QAction>
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
#include "TargetBody.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * Create an target body list from an target body list name and path (does not read TargetBody
   *  objects).
   *
   * @param name The TargetBodyList's name (i.e. import1, import2, ...)
   * @param path The TargetBodyList's folder name (i.e. import1, import2, ...)
   * @param parent The Qt-relationship parent
   */
  TargetBodyList::TargetBodyList(QString name, QString path, QObject *parent) : QObject(parent) {
    m_name = name;
    m_path = path;
  }


  /**
   * Create a blank target body list.
   *
   * @param parent The Qt-relationship parent
   */
  TargetBodyList::TargetBodyList(QObject *parent) : QObject(parent) {
  }


  /**
   * Create a target body list from a list of TargetBody's
   *
   * @param targetBodys The list of targetBody's
   * @param parent The Qt-relationship parent
   */
  TargetBodyList::TargetBodyList(QList<TargetBodyQsp> targetBodys, QObject *parent) : QObject(parent) {
    append(targetBodys);
  }


  /**
   * Create an image list from XML
   *
   * @param project The project with the target body list
   * @param xmlReader The XML reader currently at an <TargetBodyList /> tag.
   * @param parent The Qt-relationship parent
   */
  TargetBodyList::TargetBodyList(Project *project, XmlStackedHandlerReader *xmlReader,
                                 QObject *parent) : QObject(parent) {
    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  /**
   * Copy constructor.
   *
   * @param other The TargetBodyList to copy
   */
  TargetBodyList::TargetBodyList(const TargetBodyList &other) :
      QList<TargetBodyQsp>(other) {
    m_name = other.m_name;
    m_path = other.m_path;
  }


  /**
   * Create an target body list from a list of target body file names. This is slow (serial) and not
   *  recommended.
   */
//  TargetBodyList::TargetBodyList(QStringList &fileNames) {
//    foreach (QString fileName, fileNames) {
//      try {
//        TargetBodyQsp targetBody = TargetBodyQsp(new TargetBody(fileName);
//        append(targetBody);
//      }
//      catch (IException &) {
//      }
//    }
//  }


  /**
   * Destructor. This does not free the TargetBody objects from memory.
   */
  TargetBodyList::~TargetBodyList() {
  }


  /**
   * Appends a TargetBody to the list.
   *
   * @param value The TargetBody to append
   *
   * @see QList<TargetBodyQsp>::append()
   */
  void TargetBodyList::append(TargetBodyQsp const &value) {
    QList<TargetBodyQsp>::append(value);
    emit countChanged(count());
  }


  /**
   * Appends a TargetBodyList to the list.
   *
   * @param value the TargetBodyList to append
   *
   * @see QList<TargetBodyQsp>::append()
   */
  void TargetBodyList::append(const QList<TargetBodyQsp> &value) {
    QList<TargetBodyQsp>::append(value);
    emit countChanged(count());
  }


  /**
   * clears the list.
   *
   * @see QList<TargetBodyQsp>::clear()
   */
  void TargetBodyList::clear() {
    bool countChanging = count();
    QList<TargetBodyQsp>::clear();
    if (countChanging) {
      emit countChanged(count());
    }
  }


  /**
   * Erases the TargetBody associated with an iterator.
   *
   * @param pos An iterator associated with the TaretBody to be erased
   *
   * @see QList<TargetBodyQsp>::erase()
   */
  QList<TargetBodyQsp>::iterator TargetBodyList::erase(iterator pos) {
    iterator result = QList<TargetBodyQsp>::erase(pos);
    emit countChanged(count());
    return result;
  }


  /**
   * Erases all TargetBodies starting with the TargetBody associated with begin up to,
   * but not including, the TargetBody associated with end.
   *
   * @param begin An iterator associated with the first TargetBody to be erased.
   * @param end An iterator associated with the TargetBody after the last TargetBody to be erased
   *
   * @return @b QList<TargetBodyQsp>::iterator An iterator associated with the TargetBody
   *                                               end was associated with before erasing.
   *
   * @see QList<TargetBodyQsp>::erase()
   */
  QList<TargetBodyQsp>::iterator TargetBodyList::erase(iterator begin, iterator end) {
    iterator result = QList<TargetBodyQsp>::erase(begin, end);
    emit countChanged(count());
    return result;
  }


  /**
   * Inserts a TargetBody at a specific index.
   *
   * @param i The index at which to insert the TargetBody
   * @param value The TargetBody to insert
   *
   * @see QList<TargetBodyQsp>::insert()
   */
  void TargetBodyList::insert(int i, TargetBodyQsp const &value) {
    QList<TargetBodyQsp>::insert(i, value);

    emit countChanged(count());
  }


  /**
   * Inserts a TargetBody after the TargetBody associated with an iterator.
   *
   * @param before An iterator associated with the TargetBody that will be before
   *                   the inserted TargetBody
   * @param value The TargetBody to be inserted
   *
   * @return @b QList<TargetBodyQsp>::iterator An iterator associated with the inserted TargetBody
   *
   * @see QList<TargetBodyQsp>::insert()
   */
  QList<TargetBodyQsp>::iterator TargetBodyList::insert(iterator before, TargetBodyQsp const &value) {
    iterator result = QList<TargetBodyQsp>::insert(before, value);
    emit countChanged(count());
    return result;
  }


  /**
   * Inserts a TargetBody at the front of the list
   *
   * @param value the TargetBody to be inserted
   *
   * @see QList<TargetBodyQsp>::prepend()
   */
  void TargetBodyList::prepend(TargetBodyQsp const &value) {
    QList<TargetBodyQsp>::prepend(value);
    emit countChanged(count());
  }


  /**
   * Appends a TargetBody to the end of the list
   *
   * @param value The TargetBody to be append
   *
   * @see QList<TargetBodyQsp>::push_back()
   */
  void TargetBodyList::push_back(TargetBodyQsp const &value) {
    QList<TargetBodyQsp>::push_back(value);
    emit countChanged(count());
  }


  /**
   * Insertes a TargetBody at the front of the list
   *
   * @param value The TargetBody to be prepend
   *
   * @see QList<TargetBodyQsp>::push_front()
   */
  void TargetBodyList::push_front(TargetBodyQsp const &value) {
    QList<TargetBodyQsp>::push_front(value);
    emit countChanged(count());
  }


  /**
   * Removes all occurrences of a TargetBody and returns the number removed.
   *
   * @param value The TargetBody to be removed.
   *
   * @return @b int The Number of TargetBodies removed.
   *
   * @see QList<TargetBodyQsp>::removeAll()
   */
  int TargetBodyList::removeAll(TargetBodyQsp const &value) {
    int result = QList<TargetBodyQsp>::removeAll(value);

    if (result != 0) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Removes the TargetBody at a specific index.
   *
   * @param i The index of the TargetBody to be removed
   *
   * @see QList<TargetBodyQsp>::removeAt()
   */
  void TargetBodyList::removeAt(int i) {
    QList<TargetBodyQsp>::removeAt(i);
    emit countChanged(count());
  }


  /**
   * Removes the first TargetBody from the list.
   *
   * @see QList<TargetBodyQsp>::removeFirst()
   */
  void TargetBodyList::removeFirst() {
    QList<TargetBodyQsp>::removeFirst();
    emit countChanged(count());
  }


  /**
   * Removes the last TargetBody from the list.
   *
   * @see QList<TargetBodyQsp>::removeLast()
   */
  void TargetBodyList::removeLast() {
    QList<TargetBodyQsp>::removeLast();
    emit countChanged(count());
  }


  /**
   * Removes the first occurrence of a TargetBody from the list.
   *
   * @param value the TargetBody to be removed
   *
   * @return @b bool If the removal was successful
   *
   * @see QList<TargetBodyQsp>::removeOne()
   */
  bool TargetBodyList::removeOne(TargetBodyQsp const &value) {
    bool result = QList<TargetBodyQsp>::removeOne(value);

    if (result) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Swaps the list with another TargetBodyList.
   *
   * @param other The TargetBodyList to swap with
   *
   * @see QList<TargetBodyQsp>::swap()
   */
  void TargetBodyList::swap(QList<TargetBodyQsp> &other) {
    QList<TargetBodyQsp>::swap(other);

    if (count() != other.count()) {
      emit countChanged(count());
    }
  }


  /**
   * Removes and returns the TargetBody at a specific index.
   *
   * @param i The index of the TargetBody to takeAt
   *
   * @return @b TargetBodyQsp The removed TargetBody
   *
   * @see QList<TargetBodyQsp>::takeAt()
   */
  TargetBodyQsp TargetBodyList::takeAt(int i) {
    TargetBodyQsp result = QList<TargetBodyQsp>::takeAt(i);
    emit countChanged(count());
    return result;
  }


  /**
   * Removes and returns the first TargetBody in the list.
   *
   * @return @b TargetBodyQsp The first TargetBody in the list
   *
   * @see QList<TargetBodyQsp>::takeFirst()
   */
  TargetBodyQsp TargetBodyList::takeFirst() {
    TargetBodyQsp result = QList<TargetBodyQsp>::takeFirst();
    emit countChanged(count());
    return result;
  }


  /**
   * Removes and returns the last TargetBody in the list.
   *
   * @return @b TargetBodyQsp The last TargetBody in the list.
   *
   * @see QList<TargetBodyQsp>::takeLast()
   */
  TargetBodyQsp TargetBodyList::takeLast() {
    TargetBodyQsp result = QList<TargetBodyQsp>::takeLast();
    emit countChanged(count());
    return result;
  }


  /**
   * Appends another TargetBodyList to the list.
   *
   * @param other The TargetBodyList to be appended
   *
   * @return @b TargetBodyList & A reference to this list.
   *
   * @see QList<TargetBodyQsp>::operator+=()
   */
  TargetBodyList &TargetBodyList::operator+=(const QList<TargetBodyQsp> &other) {
    QList<TargetBodyQsp>::operator+=(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Appends a TargetBody to the list.
   *
   * @param other The TargetBody to be appended
   *
   * @return @b TargetBodyList & A reference to this list.
   *
   * @see QList<TargetBodyQsp>::operator+=()
   */
  TargetBodyList &TargetBodyList::operator+=(TargetBodyQsp const &other) {
    QList<TargetBodyQsp>::operator+=(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Appends another TargetBodyList to the list.
   *
   * @param other The TargetBodyList to be appended
   *
   * @return @b TargetBodyList & A reference to this list.
   *
   * @see QList<TargetBodyQsp>::operator<<()
   */
  TargetBodyList &TargetBodyList::operator<<(const QList<TargetBodyQsp> &other) {
    QList<TargetBodyQsp>::operator<<(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Appends a TargetBody to the list.
   *
   * @param other The TargetBody to be appended
   *
   * @return @b TargetBodyList & A reference to this list.
   *
   * @see QList<TargetBodyQsp>::operator<<()
   */
  TargetBodyList &TargetBodyList::operator<<(TargetBodyQsp const &other) {
    QList<TargetBodyQsp>::operator<<(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Assignment operator for a QList of TargetBodyQsp
   *
   * @param rhs The right hand side of the '=' operator
   *
   * @return @b TargetBodyList & A reference to this list
   *
   * @see QList<TargetBodyQsp>::operator=()
   */
  TargetBodyList &TargetBodyList::operator=(const QList<TargetBodyQsp> &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<TargetBodyQsp>::operator=(rhs);

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
   * @return @b TargetBodyList & A reference to this list
   */
  TargetBodyList &TargetBodyList::operator=(const TargetBodyList &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<TargetBodyQsp>::operator=(rhs);

    m_name = rhs.m_name;
    m_path = rhs.m_path;

    if (countChanging) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Gets a list of pre-connected actions that have to do with display,
   *   such as color, alpha, outline, fill, etc.
   */
//  QList<QAction *> TargetBodyList::supportedActions(Project *project) {
//    QList<QAction *> actions;

//    // It turns out connect() statements cannot be templated, hence they aren't inside of
//    //   createWorkOrder().
//    if (allSupport(ImageDisplayProperties::Color)) {
//      QAction *alphaAction = createWorkOrder(project, TargetBodyListActionWorkOrder::ChangeTransparency);
//      if (!project) {
//        connect(alphaAction, SIGNAL(triggered()),
//                this, SLOT(askAndUpdateAlpha()));
//      }
//      actions.append(alphaAction);

//      QAction *colorAction = createWorkOrder(project, TargetBodyListActionWorkOrder::ChangeColor);
//      if (!project) {
//        connect(colorAction, SIGNAL(triggered()),
//                this, SLOT(askAndUpdateColor()));
//      }
//      actions.append(colorAction);


//      QAction *ranColorAction = createWorkOrder(project, TargetBodyListActionWorkOrder::RandomizeColor);
//      if (!project) {
//        connect(ranColorAction, SIGNAL(triggered()),
//                this, SLOT(showRandomColor()));
//      }
//      actions.append(ranColorAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowLabel)) {
//      QAction *labelVisibleAction = createWorkOrder(project,
//                                                    TargetBodyListActionWorkOrder::ToggleShowLabel);
//      if (!project) {
//        connect(labelVisibleAction, SIGNAL(triggered()),
//                this, SLOT(saveAndToggleShowLabel()));
//      }
//      actions.append(labelVisibleAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowFill)) {
//      QAction *fillAction = createWorkOrder(project, TargetBodyListActionWorkOrder::ToggleShowFilled);
//      if (!project) {
//        connect(fillAction, SIGNAL(triggered()),
//                this, SLOT(saveAndToggleShowFill()));
//      }
//      actions.append(fillAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowDNs)) {
//      QAction *cubeDataAction = createWorkOrder(project,
//                                                TargetBodyListActionWorkOrder::ToggleShowCubeData);
//      if (!project) {
//        connect(cubeDataAction, SIGNAL(triggered()),
//                this, SLOT(saveAndToggleShowDNs()));
//      }
//      actions.append(cubeDataAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowOutline)) {
//      QAction *outlineAction = createWorkOrder(project,
//                                               TargetBodyListActionWorkOrder::ToggleShowOutline);
//      if (!project) {
//        connect(outlineAction, SIGNAL(triggered()),
//                this, SLOT(saveAndToggleShowOutline()));
//      }
//      actions.append(outlineAction);
//    }

//    actions.append(NULL);

//    if (!project) {
//      if (allSupport(ImageDisplayProperties::ZOrdering)) {
//        QAction *moveToTopAct = new QAction(tr("Bring to Front"), this);
//        QAction *moveUpAct = new QAction(tr("Bring Forward"), this);
//        QAction *moveToBottomAct = new QAction(tr("Send to Back"), this);
//        QAction *moveDownAct = new QAction(tr("Send Backward"), this);

//        foreach (Image *image, *this) {
//          connect(moveToTopAct, SIGNAL(triggered()),
//                  image->displayProperties(), SIGNAL(moveToTop()));

//          connect(moveUpAct, SIGNAL(triggered()),
//                  image->displayProperties(), SIGNAL(moveUpOne()));

//          connect(moveToBottomAct, SIGNAL(triggered()),
//                  image->displayProperties(), SIGNAL(moveToBottom()));

//          connect(moveDownAct, SIGNAL(triggered()),
//                  image->displayProperties(), SIGNAL(moveDownOne()));
//        }
//        actions.append(moveToTopAct);
//        actions.append(moveUpAct);
//        actions.append(moveToBottomAct);
//        actions.append(moveDownAct);
//      }

//      actions.append(NULL);

//      if (size() == 1 && allSupport(ImageDisplayProperties::Zooming)) {
//        QAction *zoomFit = new QAction(tr("Zoom Fit"), this);
//        connect(zoomFit, SIGNAL(triggered()),
//                first()->displayProperties(), SIGNAL(zoomFit()));
//        actions.append(zoomFit);
//      }
//    }

//    return actions;
//  }


  /**
   * Returns true if all of the given displays support the property
   *
   * @param prop The property we're testing for support for
   * @param displays The displays we're doing the test on
   */
//  bool TargetBodyList::allSupport(ImageDisplayProperties::Property prop) {
//    if (isEmpty())
//      return false;

//    foreach (Image *image, *this) {
//      if (!image->displayProperties()->supports(prop))
//        return false;
//    }

//    return true;
//  }


  /**
   * Set the human-readable name of this target body list. This is really only useful for project
   *   target body lists (not anonymous temporary ones).
   *
   * @param newName The name to give this target body list
   */
  void TargetBodyList::setName(QString newName) {
    m_name = newName;
  }


  /**
   * Set the relative path (from the project root) to this target body list's folder. This is really
   *  only useful for project target body lists (not anonymous temporary ones).
   *
   * @param newPath The path to the target body objects in this target body list
   */
  void TargetBodyList::setPath(QString newPath) {
    m_path = newPath;
  }


  /**
   * Get the human-readable name of this target body list
   *
   * @return @b QString The name of the target body list (or an empty string if anonymous).
   */
  QString TargetBodyList::name() const {
    return m_name;
  }


  /**
   * Get the path to these target body objects in the list (relative to project root). This only
   *  applies to a target body list from the project.
   *
   * @return @b QString The path to the target body objects in the list
   *                        (or an empty string if unknown).
   */
  QString TargetBodyList::path() const {
    return m_path;
  }


  /**
   * Delete all of the contained TargetBody objects from disk (see TargetBody::deleteFromDisk())
   */
//  void TargetBodyList::deleteFromDisk(Project *project) {
//    foreach (TargetBodyQsp targetBody, *this) {
//      targetBody->deleteFromDisk();
//    }

//    if (!m_path.isEmpty()) {
//      QFile::remove(project->imageDataRoot() + "/" + m_path + "/targets.xml");

//      QDir dir;
//      dir.rmdir(project->imageDataRoot() + "/" + m_path);
//    }
//  }


  /**
   * Convert this target body list into XML format for saving/restoring capabilities.
   *
   * This writes:
   * <pre>
   *   <TargetBodyList name="..." path="..."/>
   * </pre>
   * to the given xml stream, and creates a 'targets.xml' inside the folder with the target body
   *  objects.
   * Inside the images.xml, this writes:
   *
   * <pre>
   *   <targets>
   *     ...
   *   </targets>
   * </pre>
   *
   * @param stream The QXmlStreamWriter that will be used to convert the TargetBodyList
   * @param project The Project the TargetBodyList belongs to
   * @param newProjectRoot The root of the project folder where the TargetBodyList will be saved
   */
  void TargetBodyList::save(QXmlStreamWriter &stream, const Project *project,
                            FileName newProjectRoot) const {
//    stream.writeStartElement("TargetBodyList");
//    stream.writeAttribute("name", m_name);
//    stream.writeAttribute("path", m_path);

//    FileName settingsFileName(
//        Project::targetBodyRoot(newProjectRoot.toString()) + "/" + m_path + "/targets.xml");

//    if (!settingsFileName.dir().mkpath(settingsFileName.path())) {
//      throw IException(IException::Io,
//                       QString("Failed to create directory [%1]")
//                         .arg(settingsFileName.path()),
//                       _FILEINFO_);
//    }

//    QFile TargetBodyListContentsFile(settingsFileName.toString());

//    if (!TargetBodyListContentsFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
//      throw IException(IException::Io,
//          QString("Unable to save target body information for [%1] because [%2] could not be opened"
//                  " for writing")
//            .arg(m_name).arg(settingsFileName.original()),
//          _FILEINFO_);
//    }

//    QXmlStreamWriter targetBodyDetailsWriter(&TargetBodyListContentsFile);
//    targetBodyDetailsWriter.setAutoFormatting(true);
//    targetBodyDetailsWriter.writeStartDocument();

//    int countWidth = QString("%1L").arg(count()).size() - 1;
//    QChar paddingChar('0');

//    QLabel *progressLabel = new QLabel;

//    QProgressDialog progressDialog;
//    progressDialog.setLabel(progressLabel);
//    progressDialog.setRange(-1, count());
//    progressDialog.setValue(-1);

//    targetBodyDetailsWriter.writeStartElement("targets");
//    // Mapped is way faster than hundreds/thousands of run() calls... so use mapped for performance
//    QFuture<void *> future = QtConcurrent::mapped(*this,
//        CopyTargetBodyDataFunctor(project, newProjectRoot));

//    for (int i = 0; i < count(); i++) {
//      int newProgressValue = progressDialog.value() + 1;
//      progressLabel->setText(
//          tr("Saving Target Body Information for [%1] - %L2/%L3 done")
//            .arg(m_name)
//            .arg(newProgressValue, countWidth, 10, paddingChar)
//            .arg(count()));
//      progressDialog.setValue(newProgressValue);
//      future.resultAt(i);
//    }

//    progressLabel->setText(tr("Finalizing..."));
//    progressDialog.setRange(0, 0);
//    progressDialog.setValue(0);

//    foreach (TargetBodyQsp targetBody, *this) {
//      targetBody->save(targetBodyDetailsWriter, project, newProjectRoot);
//    }

//    targetBodyDetailsWriter.writeEndElement();

//    targetBodyDetailsWriter.writeEndDocument();

//    stream.writeEndElement();
  }


//  TargetBodyList::CopyTargetBodyDataFunctor::CopyTargetBodyDataFunctor(const Project *project,
//                                                                       FileName newProjectRoot) {
//    m_project = project;
//    m_newProjectRoot = newProjectRoot;
//  }


//  TargetBodyList::CopyTargetBodyDataFunctor::CopyTargetBodyDataFunctor(
//                  const CopyTargetBodyDataFunctor &other) {
//    m_project = other.m_project;
//    m_newProjectRoot = other.m_newProjectRoot;
//  }


//  TargetBodyList::CopyTargetBodyDataFunctor::~CopyTargetBodyDataFunctor() {
//  }


//  void *TargetBodyList::CopyTargetBodyDataFunctor::operator()(TargetBodyQsp const &targetToCopy) {
//    targetToCopy->copyToNewProjectRoot(m_project, m_newProjectRoot);
//    return NULL;
//  }


//  TargetBodyList::CopyTargetBodyDataFunctor &TargetBodyList::CopyTargetBodyDataFunctor::operator=(
//      const CopyTargetBodyDataFunctor &rhs) {
//    m_project = rhs.m_project;
//    m_newProjectRoot = rhs.m_newProjectRoot;
//    return *this;
//  }


  /**
   * Change the visibility of the display name. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
//  QStringList TargetBodyList::saveAndToggleShowLabel() {
//    QStringList results;

//    if (!isEmpty()) {
//      ImageDisplayProperties *firstDisplayProps = first()->displayProperties();
//      bool newValue = !firstDisplayProps->getValue(ImageDisplayProperties::ShowLabel).toBool();

//      foreach (Image *image, *this) {
//        ImageDisplayProperties *displayProps = image->displayProperties();

//        bool value = displayProps->getValue(ImageDisplayProperties::ShowLabel).toBool();
//        results.append(value? "shown" : "hidden");

//        image->displayProperties()->setShowLabel(newValue);
//      }
//    }

//    return results;
//  }


  /**
   * Create an XML Handler (reader) that can populate the TargetBodyList class data. See
   *   TargetBodyList::save() for the expected format.
   *
   * @param TargetBodyList The target body list we're going to be initializing
   * @param project The project that contains the target body list
   */
  TargetBodyList::XmlHandler::XmlHandler(TargetBodyList *TargetBodyList, Project *project) {
    m_TargetBodyList = TargetBodyList;
    m_project = project;
  }


  /**
   * Handle an XML start element. This expects <TargetBodyList/> and <target/> elements
   *   (it reads both the project XML and the targets.xml file).
   *
   * @param namespaceURI ???
   * @param localName The name of the element the XmlReader is at
   * @param qName ???
   * @param atts The attributes of the element the XmlReader is at
   *
   * @return @b bool If we should continue reading the XML (usually true).
   */
  bool TargetBodyList::XmlHandler::startElement(const QString &namespaceURI,
                                                const QString &localName,
                                                const QString &qName,
                                                const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "TargetBodyList") {
        QString name = atts.value("name");
        QString path = atts.value("path");

        if (!name.isEmpty()) {
          m_TargetBodyList->setName(name);
        }

        if (!path.isEmpty()) {
          m_TargetBodyList->setPath(path);
        }
      }
      else if (localName == "target") {
//        m_TargetBodyList->append(TargetBodyQsp(new TargetBody(m_project->targetBodyRoot()
//                                           + "/" + m_TargetBodyList->path(),
//                                           reader())));
      }
    }

    return true;
  }


  /**
   * Handle an XML end element. This handles <TargetBodyList /> by opening and reading the
   *   images.xml file.
   *
   * @param namespaceURI ???
   * @param localName The name of the element the XmlReader is at
   * @param qName ???
   *
   * @return @b bool If we should continue reading the XML (usually true).
   *
   * @throws IException::Io "Unable to open with read access"
   * @throws IException::Io "Failed to open target body list XML"
   */
  bool TargetBodyList::XmlHandler::endElement(const QString &namespaceURI,
                                              const QString &localName,
                                              const QString &qName) {
    if (localName == "TargetBodyList") {
      XmlHandler handler(m_TargetBodyList, m_project);

      XmlStackedHandlerReader reader;
      reader.pushContentHandler(&handler);
      reader.setErrorHandler(&handler);

      QString TargetBodyListXmlPath = m_project->targetBodyRoot() + "/" +
                                m_TargetBodyList->path() + "/targets.xml";
      QFile file(TargetBodyListXmlPath);

      if (!file.open(QFile::ReadOnly)) {
        throw IException(IException::Io,
                         QString("Unable to open [%1] with read access")
                           .arg(TargetBodyListXmlPath),
                         _FILEINFO_);
      }

      QXmlInputSource xmlInputSource(&file);
      if (!reader.parse(xmlInputSource))
        throw IException(IException::Io,
                         tr("Failed to open target body list XML [%1]").arg(TargetBodyListXmlPath),
                         _FILEINFO_);
    }

    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }
}
