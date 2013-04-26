#ifndef WorkOrder_H
#define WorkOrder_H
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

#include <QAction>
#include <QDateTime>
#include <QUndoCommand>

#include <QPointer>

// This is needed for the QVariant macro
#include <QMetaType>

#include "XmlStackedHandler.h"

template<typename T> class QFutureWatcher;
class QMutex;
class QXmlStreamWriter;

namespace Isis {
  class Control;
  class Directory;
  class ImageList;
  class ProgressBar;
  class Project;
  class XmlStackedHandlerReader;

  /**
   * @brief Parent class for anything that performs an action in Project
   *
   * This class should be used for any operation that affects a Project. This provides history,
   *   undo/redo capabilities (which need implemented correctly), and the ability for the project to
   *   guarantee a good state on disk.
   *
   * State between the end of execute() and the beginning of the redo methods must be saved via
   *   the parent (WorkOrder) class. This is to ensure serializability. State between the redo
   *   methods and undo methods should work the same way. Child implementations may only save state
   *   (have member variables) that store state between syncRedo(), asyncRedo() and postSyncRedo()
   *   OR between syncUndo(), asyncUndo() and postSyncUndo(). Other forms of state will cause the
   *   work order to not function properly when saved/restored from disk.
   *
   * @author 2012-??-?? ???
   *
   * @internal
   *   @history 2012-08-23 Steven Lambright and Stuart Sides - Updated the class to be much more
   *                           flushed out. We now have WorkOrderStatus, syncRedo(), asyncRedo(),
   *                           postSyndRedo() (also undo versions), dependsOn(), full dependency
   *                           analysis, full progress/status strings, no more race conditions in
   *                           undo/redo, the work order list is now doubly-linked, and added
   *                           the statusChanged() signal. redo() and undo() should no longer cause
   *                           any bad or undesired behavior.
   *   @history 2012-09-19 Steven Lambright - Added QList<Control *> data support for in-memory
   *                           controls. I did not yet do the serialization because we're working
   *                           on a "ControlList" class that may encapsulate some of the
   *                           implementation (maybe, we'll see).
   *   @history 2012-10-19 Steven Lambright - Removed parent argument from constructor - deleting
   *                           work orders when the creator goes away doesn't make sense. Also
   *                           work orders will work correctly when the associated images are
   *                           freed from memory and later re-allocated (the import of the images
   *                           was undone, for example). Added elapsed time value to the status text
   *                           and fixed warning for work orders without undo text.
   *                           Added listenForImageDestruction() and clearImageList().
   *   @history 2013-04-25 Jeannie Backer - Modified call to qWarning() to prevent compile warnings
   *                           on MAC OS 10.8.2
   *
   */
  class WorkOrder : public QAction, public QUndoCommand {
    Q_OBJECT
    public:
      enum WorkOrderStatus {
        WorkOrderUnknownStatus = 0,
        WorkOrderNotStarted,
        WorkOrderRedoing,
        WorkOrderRedone,
        WorkOrderUndoing,
        WorkOrderUndone,
        /**
         * This is used for work orders that will not undo or redo (See createsCleanState())
         */
        WorkOrderFinished,
        WorkOrderLastStatus = WorkOrderFinished
      };


      enum Context {
        NoContext,
        ProjectContext
      };

      WorkOrder(Project *project);
      virtual ~WorkOrder();

      virtual WorkOrder *clone() const = 0;
      virtual bool isExecutable(Context);
      virtual bool isExecutable(QList<Control *> controls);
      virtual bool isExecutable(ImageList *images);
      void read(XmlStackedHandlerReader *xmlReader);
      void save(QXmlStreamWriter &stream) const;
      virtual void setData(Context);
      virtual void setData(ImageList *images);
      virtual void setData(QList<Control *> controls);
      void setNext(WorkOrder *nextWorkOrder);
      void setPrevious(WorkOrder *previousWorkOrder);

      QString bestText() const;
      bool createsCleanState() const;
      QDateTime executionTime() const;
      bool isFinished() const;
      bool isRedoing() const;
      bool isRedone() const;
      bool isUndoing() const;
      bool isUndone() const;
      bool modifiesDiskState() const;
      WorkOrder *next() const;
      WorkOrder *previous() const;
      QString statusText() const;

      ProgressBar *progressBar();

      static WorkOrderStatus fromStatusString(QString);
      static QString toString(WorkOrderStatus);

    signals:
      void creatingProgress(WorkOrder *);
      // This is necessary because QTreeWidget doesn't support us just deleting the progress bar..
      //   HistoryWidget relies on this signal.
      void deletingProgress(WorkOrder *);
      void finished(WorkOrder *);
      void statusChanged(WorkOrder *);

    public slots:
      /**
       * The (child) implementation of this method should prompt the user/gather state by any means
       *   necessary. Prompts for file names, questions, warnings, etc.. should be done here.
       *
       * Once the work order has enough data to execute, this method needs to set the
       *   state in the parent (this) WorkOrder class. Call setData(ImageList),
       *   setInternalData(QStringList), etc... with all of the data/state necessary to perform the
       *   work order. This could be a list of file names, an ImageList of images you're viewing,
       *   or really anything else.
       *
       * Finally, the actual work needs done in *Redo(), using only state (data) stored by the
       *   parent (this) WorkOrder class. You do not have to call *Redo() - this is done for you
       *   by WorkOrder::redo().  WorkOrder::redo() is called from Project::addToProject() when the
       *   workOrder is pushed onto the undo stack.
       *
       * We do it this way to ensure saving/restoring from history
       *   can be done automatically/simply and implemented only once per data type. This also gives
       *   us full undo/redo functionality.
       *
       * @return False if this operation should be cancelled (the user clicked cancel, the operation
       *           turns out to be impossible, etc). This prevents the work order from making it
       *           into the history and redo will never be called.
       */
      virtual bool execute();

      virtual void redo();
      virtual void undo();

    protected:
      WorkOrder(const WorkOrder &other);

      ImageList *imageList();
      const ImageList *imageList() const;

      QList<Control *> controlList();

      virtual bool dependsOn(WorkOrder *other) const;

      Directory *directory() const;
      Project *project() const;

      void setCreatesCleanState(bool createsCleanState);
      void setModifiesDiskState(bool changesProjectOnDisk);
      void setInternalData(QStringList data);

      int progressMin() const;
      int progressMax() const;
      int progressValue() const;
      void setProgressRange(int, int);
      void setProgressValue(int);

      QStringList internalData() const;
      virtual void syncRedo();
      virtual void asyncRedo();
      virtual void postSyncRedo();
      virtual void syncUndo();
      virtual void asyncUndo();
      virtual void postSyncUndo();

    protected slots:
      void addCloneToProject();

    private:
      bool isInStableState() const;
      void listenForImageDestruction();
      void resetProgressBar();
      void setProgressToFinalText();

    private slots:
      void attemptQueuedAction();
      void asyncFinished();
      void clearImageList();
      void deleteProgress();
      void updateProgress();
      void startRedo();

    private:
      enum QueuedWorkOrderAction {
        NoQueuedAction,
        RedoQueuedAction,
        UndoQueuedAction
      };

      /**
       * @author 2012-??-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(WorkOrder *workOrder);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);

        private:
          Q_DISABLE_COPY(XmlHandler);

          WorkOrder *m_workOrder;
      };


    private:
      WorkOrder &operator=(const WorkOrder &rhs);

      /**
       * This is defaulted to false. If a work order saves the project to disk, this causes a
       *   'clean' (non-dirty) state. These work orders should call setCreatesCleanState(true)
       *   in their constructor.
       */
      bool m_createsCleanState;

      /**
       * This is defaulted to false. If a work order modifies the project on disk to perform its
       *   actions (for example, an import work order), the work order should call
       *   setModifiesDiskState(true) in its constructor.
       */
      bool m_modifiesDiskState;

      WorkOrderStatus m_status;
      QueuedWorkOrderAction m_queuedAction;

      int m_progressRangeMinValue;
      int m_progressRangeMaxValue;
      int m_progressValue;

      Context m_context;
      QStringList m_imageIds;
      QPointer<ImageList> m_images;
      QList<Control *> m_controls;
      QStringList m_internalData;
      QPointer<WorkOrder> m_nextWorkOrder;
      QPointer<WorkOrder> m_previousWorkOrder;
      QPointer<Project> m_project;

      QMutex *m_transparentConstMutex;

      /**
       * This is the date/time that execute() was called.
       */
      QDateTime m_executionTime;

      QPointer< QFutureWatcher<void> > m_futureWatcher;

      QPointer<ProgressBar> m_progressBar;

      QPointer<QTimer> m_progressBarUpdateTimer;
      QPointer<QTimer> m_progressBarDeletionTimer;

      QTime *m_elapsedTimer;
      double m_secondsElapsed;
  };
}

//! This allows WorkOrder *'s to be stored in a QVariant.
Q_DECLARE_METATYPE(Isis::WorkOrder *);

#endif
