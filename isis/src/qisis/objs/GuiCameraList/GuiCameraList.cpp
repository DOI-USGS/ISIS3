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
#include "GuiCameraList.h"

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
#include "GuiCamera.h"
#include "IException.h"
#include "IString.h"
#include "Project.h"

namespace Isis {
  /**
   * Create an gui camera list from a gui camera list name and path (does not read GuiCamera
   *  objects).
   *
   * @param name The GuiCameraList's name (i.e. import1, import2, ...)
   * @param path The GuiCameraList's folder name (i.e. import1, import2, ...)
   * @param parent The Qt-relationship parent
   */
  GuiCameraList::GuiCameraList(QString name, QString path, QObject *parent) : QObject(parent) {
    m_name = name;
    m_path = path;
  }


  /**
   * Create a blank gui camera list.
   *
   * @param parent The Qt-relationship parent
   */
  GuiCameraList::GuiCameraList(QObject *parent) : QObject(parent) {
  }


  /**
   * Create a gui camera list from a list of GuiCamera's
   *
   * @param guiCameras The list of guiCamera's
   * @param parent The Qt-relationship parent
   */
  GuiCameraList::GuiCameraList(QList<GuiCameraQsp> guiCameras, QObject *parent) : QObject(parent) {
    append(guiCameras);
  }


  /**
   * Copy constructor.
   *
   * @param other The GuiCameraList to copy
   */
  GuiCameraList::GuiCameraList(const GuiCameraList &src) :
      QList<GuiCameraQsp>(src) {
    m_name = src.m_name;
    m_path = src.m_path;
  }


  /**
   * Create a gui camera list from a list of gui camera file names. This is slow (serial) and not
   *  recommended.
   */
//  GuiCameraList::GuiCameraList(QStringList &fileNames) {
//    foreach (QString fileName, fileNames) {
//      try {
//        GuiCameraQsp guiCamera = GuiCameraQsp(new GuiCamera(fileName);
//        append(guiCamera);
//      }
//      catch (IException &) {
//      }
//    }
//  }


  /**
   * Destructor. This does not free the GuiCamera objects from memory.
   */
  GuiCameraList::~GuiCameraList() {
  }


  /**
   * Appends a single GuiCamera to the list.
   * 
   * @param value The GuiCamera to be appened
   * 
   * @see QList<GuiCameraQsp>::append()
   */
  void GuiCameraList::append(GuiCameraQsp const &value) {
    QList<GuiCameraQsp>::append(value);
    emit countChanged(count());
  }


  /**
   * Appends another GuiCameraList to the list.
   * 
   * @param value The GuiCameraList to be appened
   * 
   * @see QList<GuiCameraQsp>::append()
   */
  void GuiCameraList::append(const QList<GuiCameraQsp> &value) {
    QList<GuiCameraQsp>::append(value);
    emit countChanged(count());
  }


  /**
   * Clears the list.
   * 
   * @see QList<GuiCameraQsp>::clear()
   */
  void GuiCameraList::clear() {
    bool countChanging = count();
    QList<GuiCameraQsp>::clear();
    if (countChanging) {
      emit countChanged(count());
    }
  }


  /**
   * Removes the GuiCamera associated with an iterator.
   * Returns an iterator associated with the next GuiCamera in the list.
   * 
   * @param pos An iterator associated with the GuiCamera to be erased
   * 
   * @return @b QList<GuiCameraQsq>::iteraror Iterator associated with the next GuiCamera
   * 
   * @see QList<GuiCameraQsp>::erase()
   */
  QList<GuiCameraQsp>::iterator GuiCameraList::erase(iterator pos) {
    iterator result = QList<GuiCameraQsp>::erase(pos);
    emit countChanged(count());
    return result;
  }


  /**
   * Removes all GuiCameras from begin up to, but not inclusing end.
   * Returns an iterator associated with GuiCamera that was associated with end.
   * 
   * @param begin An iterator associated with the first GuiCamera to be removed
   * @param end An iterator associated with the GuiCamera after the last GuiCamera to be removed
   * 
   * @return @b QList<GuiCameraQsp>::iteraror Iterator associated with the GuiCamera that was
   *                                              associated with end.
   * 
   * @see QList<GuiCameraQsp>::erase()
   */
  QList<GuiCameraQsp>::iterator GuiCameraList::erase(iterator begin, iterator end) {
    iterator result = QList<GuiCameraQsp>::erase(begin, end);
    emit countChanged(count());
    return result;
  }


  /**
   * Inserts a GuiCamera into the list at a given index.
   * 
   * @param i The index the GuiCamera will be inserted at
   * @param value The GuiCamera to be inserted
   * 
   * @see QList<GuiCameraQsp>::insert()
   */
  void GuiCameraList::insert(int i, GuiCameraQsp const &value) {
    QList<GuiCameraQsp>::insert(i, value);

    emit countChanged(count());
  }


  /**
   * Inserts a GuiCamera in front of the GuiCamera associated with an iterator.
   * Returns an iterator associated with the inserted GuiCamera.
   * 
   * @param before An iterator associated with the GuiCamera that value will be inserted in front of
   * @param value The GuiCamera to be inserted.
   * 
   * @return @b QList<GuiCameraQsp>::iterator Iterator associated with the inserted GuiCamera
   * 
   * @see QList<GuiCameraQsp>::insert()
   */
  QList<GuiCameraQsp>::iterator GuiCameraList::insert(iterator before, GuiCameraQsp const & value) {
    iterator result = QList<GuiCameraQsp>::insert(before, value);
    emit countChanged(count());
    return result;
  }


  /**
   * Inserts a GuiCamera at the beginning of the list.
   * 
   * @param value The GuiCamera to be inserted.
   * 
   * @see QList<GuiCameraQsp>::prepend()
   */
  void GuiCameraList::prepend(GuiCameraQsp const &value) {
    QList<GuiCameraQsp>::prepend(value);
    emit countChanged(count());
  }


  /**
   * Inserts a GuiCamera at the end of the list.
   * Equivalent to append.
   * 
   * @param value The GuiCamera to be inserted.
   * 
   * @see QList<GuiCameraQsp>::push_back()
   */
  void GuiCameraList::push_back(GuiCameraQsp const &value) {
    QList<GuiCameraQsp>::push_back(value);
    emit countChanged(count());
  }


  /**
   * Inserts a GuiCamera at the front of the list.
   * Equivalent to prepend.
   * 
   * @param value The GuiCamera to be inserted.
   * 
   * @see QList<GuiCameraQsp>::push_front()
   */
  void GuiCameraList::push_front(GuiCameraQsp const &value) {
    QList<GuiCameraQsp>::push_front(value);
    emit countChanged(count());
  }


  /**
   * Removes all occurances of a GuiCamera from the list.
   * Returns the number removed.
   * 
   * @param value The GuiCamera to be removed
   * 
   * @return @b int The number of GuiCameras removed
   * 
   * @see QList<GuiCameraQsp>::removeAll()
   */
  int GuiCameraList::removeAll(GuiCameraQsp const &value) {
    int result = QList<GuiCameraQsp>::removeAll(value);

    if (result != 0) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Removes the GuiCamera at a specific index.
   * 
   * @param i The index of the GuiCamera to be removed
   * 
   * @see QList<GuiCameraQsp>::removeAt()
   */
  void GuiCameraList::removeAt(int i) {
    QList<GuiCameraQsp>::removeAt(i);
    emit countChanged(count());
  }


  /**
   * Removes the first GuiCamera in the list.
   * 
   * @see QList<GuiCameraQsp>::removeFirst()
   */
  void GuiCameraList::removeFirst() {
    QList<GuiCameraQsp>::removeFirst();
    emit countChanged(count());
  }


  /**
   * Removes the last GuiCamera in the list.
   * 
   * @see QList<GuiCameraQsp>::removeLast()
   */
  void GuiCameraList::removeLast() {
    QList<GuiCameraQsp>::removeLast();
    emit countChanged(count());
  }


  /**
   * Removes the first occurance of a GuiCamera from the list.
   * Returns whether or not it succeeded.
   * 
   * @param value The GuiCamera to be removed
   * 
   * @return @b bool True if successful
   * 
   * @see QList<GuiCameraQsp>::removeOne()
   */
  bool GuiCameraList::removeOne(GuiCameraQsp const &value) {
    bool result = QList<GuiCameraQsp>::removeOne(value);

    if (result) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * Swaps the list with another GuiCameraList.
   * 
   * @param other The GuiCameraList to swap with
   * 
   * @see QList<GuiCameraQsp>::swap()
   */
  void GuiCameraList::swap(QList<GuiCameraQsp> &other) {
    QList<GuiCameraQsp>::swap(other);

    if (count() != other.count()) {
      emit countChanged(count());
    }
  }


  /**
   * Removes and returns the GuiCamera at a specific index.
   * 
   * @param i The index of GuiCamera to be removed
   * 
   * @return @b QuiCameraQsp The GuiCamera removed
   * 
   * @see QList<GuiCameraQsp>::takeAt()
   */
  GuiCameraQsp GuiCameraList::takeAt(int i) {
    GuiCameraQsp result = QList<GuiCameraQsp>::takeAt(i);
    emit countChanged(count());
    return result;
  }


  /**
   * Removes and returns the first GuiCamera in the list.
   * 
   * @return @b GuiCameraQsp The GuiCamera removed
   * 
   * @see QList<GuiCameraQsp>::takeFirst()
   */
  GuiCameraQsp GuiCameraList::takeFirst() {
    GuiCameraQsp result = QList<GuiCameraQsp>::takeFirst();
    emit countChanged(count());
    return result;
  }


  /**
   * Removes and returns the last GuiCamera in the list.
   * 
   * @return @b GuiCameraQsp The GuiCamera removed
   * 
   * @see QList<GuiCameraQsp>::takeLast()
   */
  GuiCameraQsp GuiCameraList::takeLast() {
    GuiCameraQsp result = QList<GuiCameraQsp>::takeLast();
    emit countChanged(count());
    return result;
  }


  /**
   * Appends another GuiCameraList to the list and returns a reference to this.
   * 
   * @param other The GuiCameraList to be appended
   * 
   * @return @b QuiCameraList & A reference to this
   * 
   * @see QList<GuiCameraQsp>::operator+=()
   */
  GuiCameraList &GuiCameraList::operator+=(const QList<GuiCameraQsp> &other) {
    QList<GuiCameraQsp>::operator+=(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Appends a GuiCamera to the list and returns a reference to this.
   * 
   * @param other The GuiCamera to be appended
   * 
   * @return @b GuiCameraList & A reference to this
   * 
   * @see QList<GuiCameraQsp>::operator+=()
   */
  GuiCameraList &GuiCameraList::operator+=(GuiCameraQsp const &other) {
    QList<GuiCameraQsp>::operator+=(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Appends another GuiVameraList to the list and returns a reference to this.
   * 
   * @param other The GuiCameraList to be appened
   * 
   * @return @b GuiCameraList & A reference to this
   * 
   * @see QList<GuiCameraQsp>::operator<<()
   */
  GuiCameraList &GuiCameraList::operator<<(const QList<GuiCameraQsp> &other) {
    QList<GuiCameraQsp>::operator<<(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Appends a GuiCamera to the list and returns a reference to this.
   * 
   * @param other The GuiCamera to be appended
   * 
   * @return @b GuiCameraList & A reference to this
   * 
   * @see QList<GuiCameraQsp>::operator<<()
   */
  GuiCameraList &GuiCameraList::operator<<(GuiCameraQsp const &other) {
    QList<GuiCameraQsp>::operator<<(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * Assigns a list of GuiCameras to the list.
   * Does not modify m_name or m_path.
   * 
   * @param rhs the list being assigned from
   * 
   * @returns @b GuiCameraList & A reference to this
   * 
   * @see QList<GuiCameraQsp>::operator=()
   */
  GuiCameraList &GuiCameraList::operator=(const QList<GuiCameraQsp> &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<GuiCameraQsp>::operator=(rhs);

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
   * @return @b GuiCameraList & A reference to this
   */
  GuiCameraList &GuiCameraList::operator=(const GuiCameraList &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<GuiCameraQsp>::operator=(rhs);

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
//  QList<QAction *> GuiCameraList::supportedActions(Project *project) {
//    QList<QAction *> actions;

//    // It turns out connect() statements cannot be templated, hence they aren't inside of
//    //   createWorkOrder().
//    if (allSupport(ImageDisplayProperties::Color)) {
//      QAction *alphaAction = createWorkOrder(project, GuiCameraListActionWorkOrder::ChangeTransparency);
//      if (!project) {
//        connect(alphaAction, SIGNAL(triggered()),
//                this, SLOT(askAndUpdateAlpha()));
//      }
//      actions.append(alphaAction);

//      QAction *colorAction = createWorkOrder(project, GuiCameraListActionWorkOrder::ChangeColor);
//      if (!project) {
//        connect(colorAction, SIGNAL(triggered()),
//                this, SLOT(askAndUpdateColor()));
//      }
//      actions.append(colorAction);


//      QAction *ranColorAction = createWorkOrder(project, GuiCameraListActionWorkOrder::RandomizeColor);
//      if (!project) {
//        connect(ranColorAction, SIGNAL(triggered()),
//                this, SLOT(showRandomColor()));
//      }
//      actions.append(ranColorAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowLabel)) {
//      QAction *labelVisibleAction = createWorkOrder(project,
//                                                    GuiCameraListActionWorkOrder::ToggleShowLabel);
//      if (!project) {
//        connect(labelVisibleAction, SIGNAL(triggered()),
//                this, SLOT(saveAndToggleShowLabel()));
//      }
//      actions.append(labelVisibleAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowFill)) {
//      QAction *fillAction = createWorkOrder(project, GuiCameraListActionWorkOrder::ToggleShowFilled);
//      if (!project) {
//        connect(fillAction, SIGNAL(triggered()),
//                this, SLOT(saveAndToggleShowFill()));
//      }
//      actions.append(fillAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowDNs)) {
//      QAction *cubeDataAction = createWorkOrder(project,
//                                                GuiCameraListActionWorkOrder::ToggleShowCubeData);
//      if (!project) {
//        connect(cubeDataAction, SIGNAL(triggered()),
//                this, SLOT(saveAndToggleShowDNs()));
//      }
//      actions.append(cubeDataAction);
//    }


//    if (allSupport(ImageDisplayProperties::ShowOutline)) {
//      QAction *outlineAction = createWorkOrder(project,
//                                               GuiCameraListActionWorkOrder::ToggleShowOutline);
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
//  bool GuiCameraList::allSupport(ImageDisplayProperties::Property prop) {
//    if (isEmpty())
//      return false;

//    foreach (Image *image, *this) {
//      if (!image->displayProperties()->supports(prop))
//        return false;
//    }

//    return true;
//  }


  /**
   * Set the human-readable name of this gui camera list. This is really only useful for project
   *   gui camera lists (not anonymous temporary ones).
   *
   * @param newName The name to give this gui camera list
   */
  void GuiCameraList::setName(QString newName) {
    m_name = newName;
  }


  /**
   * Set the relative path (from the project root) to this gui camera list's folder. This is really
   *  only useful for project gui camera lists (not anonymous temporary ones).
   *
   * @param newPath The path to the gui camera objects in this gui camera list
   */
  void GuiCameraList::setPath(QString newPath) {
    m_path = newPath;
  }


  /**
   * Get the human-readable name of this gui cameray list
   *
   * @return @b QString The name of the gui camera list (or an empty string if anonymous).
   */
  QString GuiCameraList::name() const {
    return m_name;
  }


  /**
   * Get the path to these gui camera objects in the list (relative to project root). This only
   *  applies to a gui camera list from the project.
   *
   * @return @b Qstring The path to the gui camera objects in the list (or an empty string if unknown).
   */
  QString GuiCameraList::path() const {
    return m_path;
  }


  /**
   * Delete all of the contained GuiCamera objects from disk (see GuiCamera::deleteFromDisk())
   */
//  void GuiCameraList::deleteFromDisk(Project *project) {
//    foreach (GuiCameraQsp guiCamera, *this) {
//      guiCamera->deleteFromDisk();
//    }

//    if (!m_path.isEmpty()) {
//      QFile::remove(project->imageDataRoot() + "/" + m_path + "/guicameras.xml");

//      QDir dir;
//      dir.rmdir(project->imageDataRoot() + "/" + m_path);
//    }
//  }


  /**
   * Convert this gui camera list into XML format for saving/restoring capabilities.
   * Not currently implemented
   *
   * This writes:
   * <pre>
   *   <GuiCameraList name="..." path="..."/>
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
   */
  void GuiCameraList::save(QXmlStreamWriter &stream, const Project *project,
                            FileName newProjectRoot) const {
//    stream.writeStartElement("GuiCameraList");
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

//    QFile GuiCameraListContentsFile(settingsFileName.toString());

//    if (!GuiCameraListContentsFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
//      throw IException(IException::Io,
//          QString("Unable to save target body information for [%1] because [%2] could not be opened"
//                  " for writing")
//            .arg(m_name).arg(settingsFileName.original()),
//          _FILEINFO_);
//    }

//    QXmlStreamWriter targetBodyDetailsWriter(&GuiCameraListContentsFile);
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

//    foreach (GuiCameraQsp targetBody, *this) {
//      targetBody->save(targetBodyDetailsWriter, project, newProjectRoot);
//    }

//    targetBodyDetailsWriter.writeEndElement();

//    targetBodyDetailsWriter.writeEndDocument();

//    stream.writeEndElement();
  }


//  GuiCameraList::CopyTargetBodyDataFunctor::CopyTargetBodyDataFunctor(const Project *project,
//                                                                       FileName newProjectRoot) {
//    m_project = project;
//    m_newProjectRoot = newProjectRoot;
//  }


//  GuiCameraList::CopyTargetBodyDataFunctor::CopyTargetBodyDataFunctor(const CopyTargetBodyDataFunctor &other) {
//    m_project = other.m_project;
//    m_newProjectRoot = other.m_newProjectRoot;
//  }


//  GuiCameraList::CopyTargetBodyDataFunctor::~CopyTargetBodyDataFunctor() {
//  }


//  void *GuiCameraList::CopyTargetBodyDataFunctor::operator()(GuiCameraQsp const &targetToCopy) {
//    targetToCopy->copyToNewProjectRoot(m_project, m_newProjectRoot);
//    return NULL;
//  }


//  GuiCameraList::CopyTargetBodyDataFunctor &GuiCameraList::CopyTargetBodyDataFunctor::operator=(
//      const CopyTargetBodyDataFunctor &rhs) {
//    m_project = rhs.m_project;
//    m_newProjectRoot = rhs.m_newProjectRoot;
//    return *this;
//  }


  /**
   * Change the visibility of the display name. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
//  QStringList GuiCameraList::saveAndToggleShowLabel() {
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
}
