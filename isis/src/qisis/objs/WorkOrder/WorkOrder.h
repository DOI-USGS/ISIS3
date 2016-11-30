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
// This is needed for the QVariant macro
#include <QMetaType>
#include <QPointer>
#include <QUndoCommand>



#include "CorrelationMatrix.h"
#include "GuiCamera.h"
#include "TargetBody.h"
#include "XmlStackedHandler.h"

template<typename T> class QFutureWatcher;
class QMutex;
class QXmlStreamWriter;

namespace Isis {
  class Control;
  class ControlList;
  class Directory;
  class ImageList;
  class ProgressBar;
  class Project;
  class ProjectItem;
  class ShapeList;
  class XmlStackedHandlerReader;

  /**
   * @brief Parent class for anything that performs an action in Project
   *
   * This class should be used for any operation that affects a Project. This provides history,
   *   undo/redo capabilities (which need to be implemented correctly), and the ability for the
   *   project to guarantee a good state on disk.
   *
   * State between the end of execute() and the beginning of the redo methods must be saved via
   *   the parent (WorkOrder) class. This is to ensure serializability. State between the redo
   *   methods and undo methods should work the same way. Child implementations may only save state
   *   (have member variables) that store state between syncRedo(), asyncRedo() and postSyncRedo()
   *   OR between syncUndo(), asyncUndo() and postSyncUndo(). Other forms of state will cause the
   *   work order to not function properly when saved/restored from disk.
   *
   * @author 2012-??-?? Steven Lambright and Stuart Sides
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
   *   @history 2014-07-14 Kimberly Oyama - Added support for correlation matrix.
   *   @history 2015-06-12 Ken Edmundson - Added support for target body.
   *   @history 2015-10-05 Jeffrey Covington - Added support for ProjectItem.
   *                           Added new methods to support the types used by
   *                           ProjectItem. Marked old methods as deprecated.
   *   @history 2016-01-04 Jeffrey Covington - Improved support for ProjectItem.
   *   @history 2016-06-13 Tyler Wilson - Added documentation to many of the member functions
   *                          in this class.  Fixes #3956.
   *   @history 2016-06-22 Tyler Wilson - Removed all references to deprecated functions/member
   *                          variables.  Fixes #4052.
   *   @history 2016-07-26 Tracie Sucharski - Added functionality for ShapeList.
   */
  class WorkOrder : public QAction, public QUndoCommand {
    Q_OBJECT
    public:



    /**
       * @brief This enumeration is used by other functions to set and retrieve the
       * current state of the WorkOrder.
       */
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

      /**
       * @brief This enumeration is for recording the context of the current Workorder (whether
       * it is part of a project or not).
       */
      enum Context {
        NoContext,
        ProjectContext
      };

      WorkOrder(Project *project);
      virtual ~WorkOrder();

      virtual WorkOrder *clone() const = 0;

      virtual bool isExecutable(Context);
      virtual bool isExecutable(ImageList *images);
      virtual bool isExecutable(ShapeList *shapes);
      virtual bool isExecutable(ControlList *controls);
      virtual bool isExecutable(CorrelationMatrix);
      virtual bool isExecutable(TargetBodyQsp targetBody);
      virtual bool isExecutable(GuiCameraQsp guiCamera);
      virtual bool isExecutable(ProjectItem *item);

      void read(XmlStackedHandlerReader *xmlReader);
      void save(QXmlStreamWriter &stream) const;

      virtual void setData(Context);
      virtual void setData(ImageList *images);
      virtual void setData(ShapeList *shapes);
      virtual void setData(ControlList *controls);
      virtual void setData(CorrelationMatrix);
      virtual void setData(TargetBodyQsp targetBody);
      virtual void setData(GuiCameraQsp guiCamera);
      virtual void setData(ProjectItem *item);


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

      ShapeList *shapeList();
      const ShapeList *shapeList() const;

      CorrelationMatrix correlationMatrix();
      
      QPointer<ControlList> controlList();

      TargetBodyQsp targetBody();

      GuiCameraQsp guiCamera();

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
      void listenForShapeDestruction();
      void resetProgressBar();
      void setProgressToFinalText();

    private slots:
      void attemptQueuedAction();
      void asyncFinished();
      void clearImageList();
      void clearShapeList();
      void deleteProgress();
      void updateProgress();
      void startRedo();

    private:
      /**
       * @brief This enum describes the current state of a Queued WorkOrder.
       */
      enum QueuedWorkOrderAction {
        NoQueuedAction,
        RedoQueuedAction,
        UndoQueuedAction
      };

      /**        
       * @description This class is used for processing an XML file containing information
       * about a WorkOrder.
       *
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

          /**
           * @brief This is a pointer to the WorkOrder the XmlHandler is filling
           * with information it parses from an XML file.
           */
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
       * This is defaulted to false. If a WorkOrder modifies the project on disk to perform its
       * actions (for example, an import WorkOrder), the WorkOrder should call
       * setModifiesDiskState(true) in its constructor.
       */
      bool m_modifiesDiskState;

      WorkOrderStatus m_status;
      QueuedWorkOrderAction m_queuedAction;

      /**
       * The miniumum value of the Progess Bar.
       */
      int m_progressRangeMinValue;
      /**
       * The maximum value of the Progess Bar.
       */
      int m_progressRangeMaxValue;
      /**
       * The current value of the Progress Bar.
       */
      int m_progressValue;

      Context m_context;
      QPointer<ImageList> m_imageList;
      QPointer<ShapeList> m_shapeList;
      QPointer<ControlList> m_controlList;
      CorrelationMatrix m_correlationMatrix;
      /**
       * A QSharedPointer to the GuiCamera (the Camera object but encapsulated within a Gui
       * framework
       */
      GuiCameraQsp m_guiCamera;


      /**
       * A QSharedPointer to the TargetBody (A Target object but encapsulated within a Gui
       * framework.
       */
      TargetBodyQsp m_targetBody;


      /**
       * A QStringList of unique image identifiers for all of the images this WorkOrder is dealing
       * with.
       */
      QStringList m_imageIds;

      /**
       * A QStringList of unique shape identifiers for all of the shapes this WorkOrder is dealing 
       * with. 
       */
      QStringList m_shapeIds;

      /**
       * @brief A QStringList of internal properties for this WorkOrder.
       */
      QStringList m_internalData;

      /**
       * A pointer to the next WorkOrder in the queue.
       */
      QPointer<WorkOrder> m_nextWorkOrder;

      /**
       * A pointer to the previous WorkOrder in the queue.
       */
      QPointer<WorkOrder> m_previousWorkOrder;

      /**
       * A pointer to the Project this WorkOrder is attached to.
       */
      QPointer<Project> m_project;

      /**
       * This is used to protect the integrity of data the WorkOrder is working on so that only
       * one thread at a time cann access it.
       */
      QMutex *m_transparentConstMutex;

      /**
       * This is the date/time that execute() was called.
       */
      QDateTime m_executionTime;

      /**
       * @brief A pointer to a QFutureWatcher object which monitors a QFuture
       * object using signals and slots.  A QFuture object represents the results of an
       * asynchrounous operation.
       */
      QPointer< QFutureWatcher<void> > m_futureWatcher;



      /**
       * @brief A pointer to the ProgressBar.
       */
      QPointer<ProgressBar> m_progressBar;

      /**
       * @brief A pointer to the QTimer which updates the ProgressBar.
       */
      QPointer<QTimer> m_progressBarUpdateTimer;

      /**
       * @brief A pointer to the ProgressBar deletion timer.
       */
      QPointer<QTimer> m_progressBarDeletionTimer;


      /**
       * A QTime object holding the excecution time of the WorkOrder.
       */
      QTime *m_elapsedTimer;

      /**
       * @brief The seconds that have elapsed since the WorkOrder started executing.
       */
      double m_secondsElapsed;
  };
}

//! This allows WorkOrder *'s to be stored in a QVariant.
Q_DECLARE_METATYPE(Isis::WorkOrder *);

#endif
