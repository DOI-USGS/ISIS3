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
#include "IsisDebug.h"

#include "WorkOrder.h"

#include <QDebug>
#include <QFutureWatcher>
#include <QMutex>
#include <QtConcurrentRun>
#include <QTimer>
#include <QXmlStreamWriter>

#include "IException.h"
#include "IString.h"
#include "ProgressBar.h"
#include "Project.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * Create a work order that will work with the given project.
   *
   * @param project The Project that this work order should be interacting with
   * @param parent The Qt-relationship parent
   */
  WorkOrder::WorkOrder(Project *project) : QAction(project) {
    m_project = project;

    m_createsCleanState = false;
    m_modifiesDiskState = false;
    m_status = WorkOrderNotStarted;
    m_queuedAction = NoQueuedAction;
    m_transparentConstMutex = NULL;
    m_elapsedTimer = NULL;

    m_context = NoContext;
    m_images = new ImageList;
    m_futureWatcher = new QFutureWatcher<void>;
    m_transparentConstMutex = new QMutex;

    m_progressRangeMinValue = 0;
    m_progressRangeMaxValue = 100;
    m_progressValue = 0;

    m_secondsElapsed = 0.0;

    if (!m_project) {
        throw IException(IException::Programmer,
            tr("Work orders cannot be created without a project."), _FILEINFO_);
    }

    connect(this, SIGNAL(triggered()),
            this, SLOT(addCloneToProject()));
    connect(m_futureWatcher, SIGNAL(finished()),
            this, SLOT(asyncFinished()));
  }


  WorkOrder::~WorkOrder() {
    delete m_images;
    delete m_futureWatcher;
    delete m_progressBar;
    delete m_progressBarDeletionTimer;
    delete m_progressBarUpdateTimer;
    delete m_transparentConstMutex;

    m_nextWorkOrder = NULL;
    m_previousWorkOrder = NULL;
    m_project = NULL;
    m_transparentConstMutex = NULL;
  }


  /**
   * Copy the work order 'other' into this (new) instance.
   *
   * @param other The work order being copied
   */
  WorkOrder::WorkOrder(const WorkOrder &other) :
      QAction(other.icon(), ((QAction &)other).text(), other.parentWidget()),
      QUndoCommand(((QUndoCommand &)other).text()) {
    m_transparentConstMutex = NULL;
    m_elapsedTimer = NULL;

    m_project = other.project();

    m_context = other.m_context;
    m_imageIds = other.m_imageIds;
    m_images = new ImageList(*other.m_images);
    m_controls = other.m_controls;
    m_internalData = other.m_internalData;

    m_createsCleanState = other.m_createsCleanState;
    m_modifiesDiskState = other.m_modifiesDiskState;

    m_executionTime = other.m_executionTime;

    m_status = other.m_status;
    m_queuedAction = other.m_queuedAction;

    m_secondsElapsed = other.m_secondsElapsed;

    m_progressRangeMinValue = other.m_progressRangeMinValue;
    m_progressRangeMaxValue = other.m_progressRangeMaxValue;
    m_progressValue = other.m_progressValue;

    m_futureWatcher = new QFutureWatcher<void>;
    m_transparentConstMutex = new QMutex;

    if (!other.isInStableState()) {
      throw IException(IException::Unknown,
          tr("Can not copy work order [%1] because it is currently running")
            .arg(((QUndoCommand &)other).text()),
          _FILEINFO_);
    }

    connect(this, SIGNAL(triggered()),
            this, SLOT(addCloneToProject()));
    connect(m_futureWatcher, SIGNAL(finished()),
            this, SLOT(asyncFinished()));

    listenForImageDestruction();
  }


  /**
   * Re-implement this method if your work order utilizes controls for data in order to operate. For
   *   example, "CnetEditorViewWorkOrder" works sometimes on controls - the logic in side of
   *   CnetEditorViewWorkOrder::isExecutable() determines whethere or not a user is
   *   prompted with this work order as a possibility.
   */
  bool WorkOrder::isExecutable(Context context) {
    return false;
  }


  /**
   * Re-implement this method if your work order utilizes controls for data in order to operate. For
   *   example, "CnetEditorViewWorkOrder" works sometimes on controls - the logic in side of
   *   CnetEditorViewWorkOrder::isExecutable() determines whethere or not a user is
   *   prompted with this work order as a possibility.
   *
   */
  bool WorkOrder::isExecutable(QList<Control *> controls) {
    return false;
  }


  /**
   * Re-implement this method if your work order utilizes images for data in order to operate. For
   *   example, "Footprint2DViewWorkOrder" works sometimes on images - the logic in side of
   *   Footprint2DViewWorkOrder::isExecutable(ImageList) determines whethere or not a user is
   *   prompted with this work order as a possibility.
   *
   * @param images An image list that this work order should execute on
   */
  bool WorkOrder::isExecutable(ImageList *images) {
    return false;
  }


  /**
   * Read this work order's data from disk.
   */
  void WorkOrder::read(XmlStackedHandlerReader *xmlReader) {
    xmlReader->pushContentHandler(new XmlHandler(this));
  }


  /**
   * Output XML format:
   *
   * <pre>
   *   <workOrder actionText="..." undoText="..." type="..." status="...">
   *     <images>
   *       <image id="..." />
   *     </images>
   *
   *     <internalDataValues>
   *       <dataValue value="..." />
   *     </internalDataValues>
   *   </workOrder>
   * </pre>
   */
  void WorkOrder::save(QXmlStreamWriter &stream) const {
    if (!isInStableState()) {
      throw IException(IException::Unknown,
                       tr("Can not store an unstable work order. The work order [%1] is currently "
                          "working").arg(bestText()),
                       _FILEINFO_);
    }

    stream.writeStartElement("workOrder");

    stream.writeAttribute("actionText", QAction::text());
    stream.writeAttribute("undoText", QUndoCommand::text());
    stream.writeAttribute("executionTime", m_executionTime.toString());
    stream.writeAttribute("type", metaObject()->className());
    stream.writeAttribute("status", toString(m_status));

    if (m_imageIds.count()) {
      stream.writeStartElement("images");

      foreach (QString imageId, m_imageIds) {
        stream.writeStartElement("image");
        stream.writeAttribute("id", imageId);
        stream.writeEndElement();
      }

      stream.writeEndElement();
    }

    if (m_internalData.count()) {
      stream.writeStartElement("internalDataValues");

      foreach (QString str, m_internalData) {
        stream.writeStartElement("dataValue");
        stream.writeAttribute("value", str);
        stream.writeEndElement();
      }

      stream.writeEndElement();
    }

    if (m_context != NoContext) {
      stream.writeStartElement("context");

      QString contextStr = "ProjectContext";
      stream.writeAttribute("value", contextStr);

      stream.writeEndElement();
    }

    stream.writeEndElement();
  }


  void WorkOrder::setData(Context context) {
    m_context = context;
  }


  void WorkOrder::setData(ImageList *images) {
    m_imageIds.clear();
    delete m_images;

    m_images = new ImageList(*images);
    listenForImageDestruction();
  }


  void WorkOrder::setData(QList<Control *> controls) {
    m_controls = controls;
  }


  void WorkOrder::setNext(WorkOrder *nextWorkOrder) {
    m_nextWorkOrder = nextWorkOrder;
  }


  void WorkOrder::setPrevious(WorkOrder *previousWorkOrder) {
    m_previousWorkOrder = previousWorkOrder;
  }


  ImageList *WorkOrder::imageList() {
    if (!m_images) {
      bool anyImagesAreNull = false;

      m_images = new ImageList;

      foreach (QString id, m_imageIds) {
        Image *image = project()->image(id);
        m_images->append(image);

        if (!image) {
          anyImagesAreNull = true;
        }
      }

      if (anyImagesAreNull) {
        delete m_images;
      }
      else {
        listenForImageDestruction();
      }
    }

    return m_images;
  }


  QList<Control *> WorkOrder::controlList() {
    return m_controls;
  }


  const ImageList *WorkOrder::imageList() const {
    QMutexLocker lock(m_transparentConstMutex);
    return const_cast<WorkOrder *>(this)->imageList();
  }


  bool WorkOrder::dependsOn(WorkOrder *) const {
    return true;
  }


  QString WorkOrder::bestText() const {
    QString result = QUndoCommand::text().remove("&").remove("...");

    // We don't use action text anymore because Directory likes to rename our actions... it
    //   converts a set of actions that have the same text, like Zoom Fit, to be in a menu named
    //   Zoom Fit with items that name their widgets. Widget names are unhelpful as a description
    //   of the action.
    //
    // See Directory::restructureActions
    //

    // if the QUndoCommand has no text, create a warning
    if (result.isEmpty()) {
      // get the name of the work order
      result = QString(metaObject()->className()).remove("Isis::").remove("WorkOrder")
                   .replace(QRegExp("([a-z0-9])([A-Z])"), "\\1 \\2");
      qWarning() << QString("WorkOrder::bestText(): Work order [%1] has no QUndoCommand text").arg(result);
    }

    return result;
  }


  bool WorkOrder::createsCleanState() const {
    return m_createsCleanState;
  }


  QDateTime WorkOrder::executionTime() const {
    return m_executionTime;
  }


  bool WorkOrder::isFinished() const {
    return m_status == WorkOrderFinished;
  }


  bool WorkOrder::isRedoing() const {
    return m_status == WorkOrderRedoing;
  }


  bool WorkOrder::isRedone() const {
    return m_status == WorkOrderRedone;
  }


  bool WorkOrder::isUndoing() const {
    return m_status == WorkOrderUndoing;
  }


  bool WorkOrder::isUndone() const {
    return m_status == WorkOrderUndone;
  }


  bool WorkOrder::modifiesDiskState() const {
    return m_modifiesDiskState;
  }


  WorkOrder *WorkOrder::next() const {
    return m_nextWorkOrder;
  }


  WorkOrder *WorkOrder::previous() const {
    return m_previousWorkOrder;
  }


  QString WorkOrder::statusText() const {
    QString result = toString(m_status);

    if (m_secondsElapsed) {
      // QTime can't format in the way that I want (0-n minutes, 00-59 seconds, no hours
      //   displayed)... so do it manually.
      // Expected output format examples: 0:01, 0:55, 1:30, 55:55, 90:00, 100:12
      int seconds = qRound(m_secondsElapsed) % 60;
      int minutes = qRound(m_secondsElapsed) / 60;
      result += tr(" (elapsed: %1:%2)").arg(minutes).arg(seconds, 2, 10, QChar('0'));
    }

    return result;
  }


  ProgressBar *WorkOrder::progressBar() {
    return m_progressBar;
  }


  WorkOrder::WorkOrderStatus WorkOrder::fromStatusString(QString statusString) {
    statusString = statusString.toUpper();
    WorkOrderStatus result = WorkOrderUnknownStatus;

    for (WorkOrderStatus possibleResult = WorkOrderUnknownStatus;
         possibleResult <= WorkOrderLastStatus;
         possibleResult = (WorkOrderStatus)(possibleResult + 1)) {
      if (statusString == toString(possibleResult).toUpper()) {
        result = possibleResult;
      }
    }

    return result;
  }


  QString WorkOrder::toString(WorkOrderStatus status) {
    QString result;

    switch (status) {
      case WorkOrderUnknownStatus:
        result = tr("Unknown");
        break;
      case WorkOrderNotStarted:
        result = tr("Not Started");
        break;
      case WorkOrderRedoing:
        result = tr("In Progress");
        break;
      case WorkOrderRedone:
        result = tr("Completed");
        break;
      case WorkOrderUndoing:
        result = tr("Undoing");
        break;
      case WorkOrderUndone:
        result = tr("Undone");
        break;
      case WorkOrderFinished:
        result = tr("Finished");
        break;
    }

    return result;
  }


  /**
   * Starts (or enqueues) a redo. This should not be re-implemented by children.
   */
  void WorkOrder::redo() {
    if (!isInStableState()) {
      m_queuedAction = RedoQueuedAction;
    }

    if (!isRedone()) {
      bool mustQueueThisRedo = false;

      WorkOrder *dependency = NULL;
      WorkOrder *current = this;
      while (current->previous() && !dependency) {
        if (!current->previous()->isRedone() && !current->previous()->isFinished()) {
          WorkOrder *possibleDependency = current->previous();

          if (dependsOn(possibleDependency)) {
            connect(possibleDependency, SIGNAL(finished(WorkOrder *)),
                    this, SLOT(attemptQueuedAction()));
            dependency = possibleDependency;
            mustQueueThisRedo = true;
          }
        }

        current = current->previous();
      }

      if (!imageList()) {
        connect(project(), SIGNAL(imagesAdded(ImageList *)),
                this, SLOT(attemptQueuedAction()));
        mustQueueThisRedo = true;
      }

      if (mustQueueThisRedo && !isUndoing() && !isRedoing()) {
        m_queuedAction = RedoQueuedAction;

        QString queueStatusText;

        if (dependency) {
          QString dependencyText = dependency->bestText();

          if (dependencyText.count() > 5) {
            dependencyText = dependencyText.mid(0, 5) + "...";
          }

          queueStatusText = tr("Wait for [%1]").arg(dependencyText);
        }
        else if (!imageList()) {
          queueStatusText = tr("Wait for images");
        }

        resetProgressBar();
        m_progressBar->setValue(m_progressBar->minimum());
        m_progressBar->setText(queueStatusText);
        m_progressBar->update();
      }

      if (m_queuedAction == NoQueuedAction) {
        m_status = WorkOrderRedoing;
        emit statusChanged(this);

        resetProgressBar();
        m_progressBar->setText("Starting...");
        m_progressBar->update();

        delete m_elapsedTimer;
        m_elapsedTimer = new QTime;
        m_elapsedTimer->start();

        syncRedo();

        m_progressBar->setText("Running...");
        m_progressBar->update();
        QFuture<void> future = QtConcurrent::run(this, &WorkOrder::asyncRedo);
        m_futureWatcher->setFuture(future);
      }
    }
    else {
      setProgressToFinalText();
    }
  }


  /**
   * Starts (or enqueues) an undo. This should not be re-implemented by children.
   */
  void WorkOrder::undo() {
    if (!isInStableState()) {
      m_queuedAction = UndoQueuedAction;
    }

    if (!isUndone() && m_status != WorkOrderNotStarted) {
      WorkOrder *dependency = NULL;
      WorkOrder *current = this;
      while (current->next() && !dependency) {
        if (!current->next()->isUndone() && !current->next()->isFinished() &&
            current->next()->m_status != WorkOrderNotStarted) {
          connect(current->next(), SIGNAL(finished(WorkOrder *)),
                  this, SLOT(attemptQueuedAction()));
          m_queuedAction = UndoQueuedAction;
          dependency = current->next();
        }

        current = current->next();
      }

      if (dependency && !isUndoing() && !isRedoing()) {
        QString prevText = dependency->bestText();

        if (prevText.count() > 5) {
          prevText = prevText.mid(0, 5) + "...";
        }

        resetProgressBar();
        m_progressBar->setValue(m_progressBar->minimum());
        m_progressBar->setText(tr("Undo after [%1]").arg(prevText));
        m_progressBar->update();
      }

      if (m_queuedAction == NoQueuedAction) {
        m_status = WorkOrderUndoing;
        emit statusChanged(this);

        resetProgressBar();
        m_progressBar->setText("Starting Undo...");
        m_progressBar->update();

        delete m_elapsedTimer;
        m_elapsedTimer = new QTime;
        m_elapsedTimer->start();

        syncUndo();

        m_progressBar->setText("Undoing...");
        m_progressBar->update();
        QFuture<void> future = QtConcurrent::run(this, &WorkOrder::asyncUndo);
        m_futureWatcher->setFuture(future);
      }
    }
    else {
      setProgressToFinalText();
    }
  }


  /**
   * This method is designed to be implemented by children work orders, but they need to call this
   *   version inside of their execute (at the beginning). The order of execution for
   *     work orders is:
   *   execute() - GUI thread, can ask user for input*
   *   syncRedo() - GUI thread, should not prompt the user for input
   *   asyncRedo() - Pooled thread
   *   postSyncRedo() - GUI thread
   *
   *   syncUndo() - GUI thread, always called after redo finishes
   *   asyncUndo() - Pooled thread
   *   postSyncUndo() - GUI thread
   *
   *   syncRedo() - GUI thread
   *   asyncRedo() - Pooled thread
   *   postSyncRedo() - GUI thread
   *
   *   and so on...
   *
   * State should only be set in the parent WorkOrder class in this method. You can set arbitrary
   *   state using setInternalData(). This method is always executed in the GUI thread and is the
   *   only place to ask the user questions.
   */
  bool WorkOrder::execute() {
    // We're finished at this point if we save/open a project, we're not finished if we need to do
    //   redo()
    if (createsCleanState()) {
      m_status = WorkOrderFinished;

      emit statusChanged(this);
    }

    m_executionTime = QDateTime::currentDateTime();

    resetProgressBar();

    if (createsCleanState()) {
      setProgressToFinalText();
    }
    else {
      m_progressBar->setText("Initializing...");
    }

    return true;
  }


  Directory *WorkOrder::directory() const {
    return project()->directory();
  }


  Project *WorkOrder::project() const {
    if (!m_project) {
      throw IException(IException::Programmer,
          "This work order no longer has a project.", _FILEINFO_);
    }

    return m_project;
  }


  void WorkOrder::setInternalData(QStringList data) {
    m_internalData = data;
  }


  int WorkOrder::progressMin() const {
    return m_progressRangeMinValue;
  }


  int WorkOrder::progressMax() const {
    return m_progressRangeMaxValue;
  }


  int WorkOrder::progressValue() const {
    return m_progressValue;
  }


  void WorkOrder::setProgressRange(int minValue, int maxValue) {
    m_progressRangeMinValue = minValue;
    m_progressRangeMaxValue = maxValue;
  }


  void WorkOrder::setProgressValue(int value) {
    m_progressValue = value;
  }


  QStringList WorkOrder::internalData() const {
    return m_internalData;
  }


  /**
   * This method is designed to be implemented by children work orders. The order of execution for
   *     redo is:
   *   syncRedo() - GUI thread*
   *   asyncRedo() - Pooled thread
   *   postSyncRedo() - GUI thread
   *
   * State should only be read from the parent WorkOrder class in this method. You can set state to
   *   be used in asyncRedo() and postSyncRedo() safely. This method is always executed in the GUI
   *   thread and has no progress.
   */
  void WorkOrder::syncRedo() {
  }


  /**
   * This method is designed to be implemented by children work orders. The order of execution for
   *     redo is:
   *   syncRedo() - GUI thread
   *   asyncRedo() - Pooled thread*
   *   postSyncRedo() - GUI thread
   *
   * State can be read from the parent WorkOrder class and from state set in syncRedo() while in
   *   this method. You can set state to be used in postSyncRedo() safely. Please be wary of
   *   creating QObjects inside of this method because they will associated with the pooled thread
   *   and must be moved back to the GUI thread with QObject::moveToThread(). This method is never
   *   executed in the GUI thread. You can update progress by calling setProgressRange() and
   *   setProgressValue(). Please do not manipulate any GUI objects here.
   */
  void WorkOrder::asyncRedo() {
  }


  /**
   * This method is designed to be implemented by children work orders. The order of execution for
   *     redo is:
   *   syncRedo() - GUI thread
   *   asyncRedo() - Pooled thread
   *   postSyncRedo() - GUI thread*
   *
   * State can be read from the parent WorkOrder class and from state set in either syncRedo() or
   *   asyncRedo() while in this method. You can not set state to be used in any of the undo code
   *   safely. This method is always executed in the GUI thread and has no progress.
   */
  void WorkOrder::postSyncRedo() {
  }


  /**
   * This method is designed to be implemented by children work orders. The order of execution for
   *     undo is:
   *   syncUndo() - GUI thread*
   *   asyncUndo() - Pooled thread
   *   postSyncUndo() - GUI thread
   *
   * State should only be read from the parent WorkOrder class in this method. You can set state to
   *   be used in asyncUndo() and postSyncUndo() safely. This method is always executed in the GUI
   *   thread and has no progress.
   */
  void WorkOrder::syncUndo() {
  }


  /**
   * This method is designed to be implemented by children work orders. The order of execution for
   *     undo is:
   *   syncUndo() - GUI thread
   *   asyncUndo() - Pooled thread*
   *   postSyncUndo() - GUI thread
   *
   * State can be read from the parent WorkOrder class and from state set in syncUndo() while in
   *   this method. You can set state to be used in postSyncUndo() safely. Please be wary of
   *   deleting QObjects inside of this method because they will cause unpredictable crashes. This
   *   method is never executed in the GUI thread. You can update progress by calling
   *   setProgressRange() and setProgressValue(). Please do not manipulate any GUI objects here.
   */
  void WorkOrder::asyncUndo() {
  }


  /**
   * This method is designed to be implemented by children work orders. The order of execution for
   *     undo is:
   *   syncUndo() - GUI thread
   *   asyncUndo() - Pooled thread
   *   postSyncUndo() - GUI thread*
   *
   * State can be read from the parent WorkOrder class and from state set in either syncUndo() or
   *   asyncUndo() while in this method. You can not set state to be used in any of the redo code
   *   safely. This method is always executed in the GUI thread and has no progress.
   */
  void WorkOrder::postSyncUndo() {
  }


  void WorkOrder::addCloneToProject() {
    if (project()) {
      project()->addToProject(clone());
    }
  }


  bool WorkOrder::isInStableState() const {
    bool result = true;

    if (isRedoing() || isUndoing() || m_queuedAction != NoQueuedAction) {
      result = false;
    }

    return result;
  }


  void WorkOrder::listenForImageDestruction() {
    m_imageIds.clear();
    foreach (Image *image, *m_images) {
      if (image) {
        m_imageIds.append(image->id());

        // If we lose any images, destroy the entire list. This will let us know that we need to
        //   rebuild it, if needed, when requested.
        connect(image, SIGNAL(destroyed(QObject *)),
                this, SLOT(clearImageList()));
      }
    }
  }


  void WorkOrder::resetProgressBar() {
    delete m_progressBarDeletionTimer;

    if (!m_progressBar) {
      m_progressBar = new ProgressBar;
      emit creatingProgress(this);
    }

    if (!m_progressBarUpdateTimer) {
      m_progressBarUpdateTimer = new QTimer;
      connect(m_progressBarUpdateTimer, SIGNAL(timeout()),
              this, SLOT(updateProgress()));
      m_progressBarUpdateTimer->start(100);
    }

    m_progressRangeMinValue = 0;
    m_progressRangeMaxValue = 100;
    m_progressValue = 0;
  }


  void WorkOrder::setProgressToFinalText() {
    if (m_progressBar) {
      if (isRedone()) {
        m_progressBar->setText(tr("Finished"));
      }
      else if (isUndone() || m_status == WorkOrderNotStarted) {
        m_progressBar->setText(tr("Undone"));
      }

      if (m_progressBar->minimum() != 0 || m_progressBar->maximum() != 0) {
        m_progressBar->setValue(m_progressBar->maximum());
      }
      else {
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(100);
      }

      delete m_progressBarDeletionTimer;
      m_progressBarDeletionTimer = new QTimer;
      m_progressBarDeletionTimer->setSingleShot(true);
      connect(m_progressBarDeletionTimer, SIGNAL(timeout()),
              this, SLOT(deleteProgress()));

      m_progressBarDeletionTimer->start(5 * 1000); // 5 seconds

      m_progressBar->update();
    }
  }


  void WorkOrder::attemptQueuedAction() {
    QueuedWorkOrderAction queued = m_queuedAction;
    m_queuedAction = NoQueuedAction;

    if (queued == RedoQueuedAction && m_status != WorkOrderRedone) {
      redo();
    }
    else if (queued == UndoQueuedAction && m_status != WorkOrderUndone) {
      undo();
    }
  }


  void WorkOrder::asyncFinished() {
    delete m_progressBarUpdateTimer;

    WorkOrderStatus finishedStatus = WorkOrderRedone;
    void (WorkOrder::*postSyncMethod)() = &WorkOrder::postSyncRedo;

    if (isUndoing()) {
      finishedStatus = WorkOrderUndone;
      postSyncMethod = &WorkOrder::postSyncUndo;
    }

    (this->*postSyncMethod)();

    m_status = finishedStatus;

    m_secondsElapsed = m_elapsedTimer->elapsed() / 1000.0;

    delete m_elapsedTimer;
    m_elapsedTimer = NULL;

    emit statusChanged(this);
    setProgressToFinalText();
    emit finished(this);

    attemptQueuedAction();
  }


  void WorkOrder::clearImageList() {
    delete m_images;
  }


  void WorkOrder::deleteProgress() {
    ProgressBar *progress = m_progressBar;

    if (m_progressBar) {
      m_progressBar = NULL;
      emit deletingProgress(this);
      delete progress;
    }
  }


  void WorkOrder::updateProgress() {
    if (m_progressBar && (isRedoing() || isUndoing())) {
      m_progressBar->setRange(m_progressRangeMinValue, m_progressRangeMaxValue);
      m_progressBar->setValue(m_progressValue);
    }
  }


  void WorkOrder::startRedo() {
  }


  /**
   * Declare that this work order is saving the project. This makes the work order not appear in
   *   the undo stack (cannot undo/redo), and instead is marked as a 'clean' state of the project.
   *   The QUndoCommand undo/redo will never be called. The default for createsCleanState is false.
   *
   * @param createsCleanState True if this work order is going to save the project to disk
   */
  void WorkOrder::setCreatesCleanState(bool createsCleanState) {
    m_createsCleanState = createsCleanState;
  }


  void WorkOrder::setModifiesDiskState(bool changesProjectOnDisk) {
    m_modifiesDiskState = changesProjectOnDisk;
  }


  WorkOrder::XmlHandler::XmlHandler(WorkOrder *workOrder) {
    m_workOrder = workOrder;
  }


  /**
   * Handle an XML start element. This expects <workOrder/> and <dataValue/> elements.
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool WorkOrder::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "workOrder") {
        QString actionText = atts.value("actionText");
        QString undoText = atts.value("undoText");
        QString executionTime = atts.value("executionTime");
        QString statusStr = atts.value("status");

        if (!actionText.isEmpty()) {
          ((QAction *)m_workOrder)->setText(actionText);
        }

        if (!undoText.isEmpty()) {
          ((QUndoCommand *)m_workOrder)->setText(undoText);
        }

        if (!executionTime.isEmpty()) {
          m_workOrder->m_executionTime = QDateTime::fromString(executionTime);
        }

        if (!statusStr.isEmpty()) {
          m_workOrder->m_status = fromStatusString(statusStr);
        }
        else {
          if (m_workOrder->createsCleanState()) {
            m_workOrder->m_status = WorkOrderFinished;
          }
          else {
            m_workOrder->m_status = WorkOrderRedone;
          }
        }
      }
      else if (localName == "dataValue") {
        m_workOrder->m_internalData.append(atts.value("value"));
      }
      else if (localName == "context") {
        if (atts.value("value") == "ProjectContext") {
          m_workOrder->m_context = ProjectContext;
        }
      }
    }

    return true;
  }
}
