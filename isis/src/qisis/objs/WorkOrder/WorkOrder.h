#ifndef WorkOrder_H
#define WorkOrder_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QAction>
#include <QDateTime>
// This is needed for the QVariant macro
#include <QMetaType>
#include <QPointer>
#include <QUndoCommand>
#include <QElapsedTimer>



#include "CorrelationMatrix.h"
#include "FileItem.h"
#include "GuiCamera.h"
#include "TargetBody.h"

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
  class Template;

  /**
   * @brief Provide Undo/redo abilities, serialization, and history for an operation.
   *
   *   This class should be used for operations that affect a Project and need to provide history
   *   and/or undo/redo capabilities, and the ability for the project to guarantee a
   *   good state on disk. It follows the Command Pattern using Qt's QUndoCommand framework.
   *   Not all actions require WorkOrders - many of the
   *   actions performed in the various widgets may not use WorkOrders.
   *
   *   The order of execution for work orders is:
   *   setupExecution() - GUI thread, can ask user for input
   *   execute() - run on either the GUI thread or a non-GUI thread as specified by the m_isSynchronous flag
   *   postExecution() - perform any cleanup after execute.
   *
   *
   *   undoExecution() - run on either the GUI thread or a non-GUI thread as specified by the
   *                     m_isSynchronous flag
   *   postUndoExecution() - perform any cleanup after undoExecution()
   *
   *
   *   ** Adding a new Workorder **
   *
   *   The WorkOrder will need to be determined to be either synchronous/asynchronous and
   *   whether it is undoable.  These are decisions determined by the use case.  Asynchronous
   *   WorkOrders will not block the GUI thread while running and are typically used for
   *   long-running operations.  Note that WorkOrders are not reentrant - a new one is created
   *   for each action.
   *
   *   The constructor for the WorkOrder must set m_isUndoable and m_isSynchronous to the appropriate
   *   values. The constructor must call the base WorkOrder constructor.  The default is
   *   synchronous and undoable. If an import WorkOrder is being implemented the import must be
   *   some type of object and implement certain slots.
   *
   *   All information required to execute the WorkOrder should be saved in the WorkOrder
   *   in the setupExecution() method.  Since WorkOrders may be serialized and may run on
   *   non-GUI threads there are restrictions on how the WorkOrder may save state.  To allow
   *   serialization the WorkOrders  must save state to the base WorkOrder class using
   *   WorkOrder::setInternalData() in the following calls:
   *   setupExecution(), postExecution(), postUndoExecution().
   *   Workorders may use member variables to pass data between the execute() and postExecution()
   *   methods and also between the undoExecution() and undoPostExecution() methods since
   *   serialization can not happen between these calls.  For asynchronous WorkOrders the
   *   execute()/postExecution() and undoExecution()/undoPostExecution() methods are on
   *   different threads so any allocated memory moved between the non-GUI and GUI threads
   *   between methods.
   *
   *   Serialization is handled by the base WorkOrder class.  Since all state is saved
   *   into the base class using setInternalData() the derived WorkOrders do not contain
   *   any data that needs to be serialized.  The times when WorkOrders are allowed to use
   *   member variables are periods when the WorkOrder can not be serialized.
   *
   *   There are 5 key methods in the flow of the WorkOrder as shown in the WorkOrder Flow
   *   diagram below.
   *
   *   *setupExecution*
   *   The setupExecution() method gathers all required information to run the WorkOrder but
   *   does not execute it.  The gathered information is stored in the WorkOrder.
   *   SetupExecution() is optional but typically required. It can bring up GUI elements to
   *   prompt the user for any necessary information.  SetupExecution() is not called when
   *   a WorkOrder is redone.
   *
   *   *execute*
   *   execute() needs to be implemented perform the WorkOrder.
   *   All information neccessary to run the WorkOrder should already be stored in the WorkOrder.
   *   The data necessary for the  WorkOrder can be retrieved via internalData()
   *   Execute() can not use any GUI elements. Each time a
   *   WorkOrder is redone execute() is called to redo the WorkOrder.
   *
   *   For synchronous WorkOrders the execute()
   *   method runs on the GUI thread and there are no special requirements on object ownership.
   *
   *   For asynchronous WorkOrders any memory allocations that aren't deallocated within
   *   execute() will need to be moved to the GUI thread.  @see ImportImagesWorkOrder::execute
   *   for an example of an asynchronous WorkOrder that allocates memory. setProgressValue()
   *   can be used to update the progress bar in the GUI. Any member variables being accessed by an
   *   asynchronous workorder will need to have a QMutex locker so they can be thread safe
   *
   *   *postExecution*
   *   postExecution() runs on the GUI thread so it should not perform any long running operations.
   *   It is intended for any cleanup or GUI updates after execute().  Typically it would only be
   *   needed for asynchronous WorkOrders where they need to update the GUI and do cleanup.  It is
   *   not required to implement this method.
   *
   *   *undoExecution*
   *   undoExecution() is only required for undoable WorkOrders.  undoExecution() should undo the
   *   effects of the execute() only using state stored in the
   *   WorkOrder.  It will run on the GUI thread if synchronous or a non-GUI thread if asynchronous.
   *   The same restrictions as execute() apply to this method.
   *
   *   *postUndoExecution()*
   *   This is not required. If needed, it should perform any cleanup after undoExecution().
   *   postUndoExecution() has the same restrictions as postExecution().
   *
   *   Other methods the WorkOrder may need to implement are:
   *
   *   *isExecutable(<various type>)*
   *   IsExecutable() determines if the WorkOrder should show up in the context menus (this has no
   *   bearing on how the main menu is populated).  Note that isExecutable will need to be
   *   implemented for each type of parameter this WorkOrder should show in the context menu.
   *
   *   *dependsOn*
   *   This is currently not implemented properly for most WorkOrders.  In theory this should determine
   *   if the workOrder parameter passed in must be completed prior to this workOrder completing.   Most
   *   current WorkOrders just check if the WorkOrder parameter is the same type.
   *
   *   *setCreatesCleanState*
   *   This is used to indicate the WorkOrder has set the state back to an unchanged state from which
   *   the project was originally opened.  This is used by open, save, and close project WorkOrders. Unlikely
   *   to be needed by other WorkOrders. This can needs to be set to false to be able to have an undoable
   *   workorder
   *
   *   *setModifiesDiskState*
   *   WorkOrders should call this to indicate they modify the disk state, this should be set to true to
   *   be able to have an undoable workorder.  The WorkOrder should implement the undoExecution method
   *   if this is set to true.
   *
   *
   *   **WorkOrder Diagrams**
   *
   * @startuml {workOrderFlow.png} "WorkOrder Flow"
   * |GUI thread|
   * start
   *  #yellow:User selects workorder from menu<
   * if (workOrder::setupExecution()) then (true)
   *  repeat
   *   if (WorkOrder::isSynchronous()) then (true)
   *     :WorkOrder::execute();
   *   else (false)
   *     |non-GUI thread|
   *     :WorkOrder::execute();
   *     |GUI thread|
   *   endif
   *   :WorkOrder::postExecute();
   *
   *   #yellow:User selects undo from menu<
   *
   *   if (WorkOrder::isSynchronous()) then (true)
   *     :WorkOrder::undoExecution();
   *   else (false)
   *     |non-GUI thread|
   *     :WorkOrder::undoExecution();
   *    |GUI thread|
   *  endif
   *  :WorkOrder::postUndoExecution();
   *
   *  #yellow:User selects redo from menu<
   * repeat while (test)
   *
   * else (false)
   *   stop
   * endif
   * stop
   * @enduml
   *
   *@startuml {NonUndoableWorkOrderSequence.png} "Non-undoable WorkOrder Sequence (unfinished)"
   * title Non-undoable Workorder (unfinished)
   * actor User
   * participant WorkOrder
   * participant Project
   * participant HistoryTreeWidget
   *
   * User -> WorkOrder: Menuclick
   *
   * activate WorkOrder
   *
   * WorkOrder -> WorkOrder: addCloneToProject
   * activate WorkOrder
   *
   * WorkOrder -> Project : addToProject
   * activate Project
   *
   *  Project -> WorkOrder : setPrevious
   *
   * Project -> WorkOrder : **setupExecution**
   * activate WorkOrder
   * WorkOrder -> Project
   * deactivate WorkOrder
   *
   *  Project -> WorkOrder : setNextWorkorder
   *
   * Project -> HistoryTreeWidget : << signal:workOrderStarting >>
   * Project <-- HistoryTreeWidget : slot:addToHistory
   *
   * Project -> WorkOrder: **execute**
   * activate WorkOrder
   * WorkOrder -> Project
   * deactivate WorkOrder
   *
   * Project  -> WorkOrder
   * deactivate Project
   *
   * deactivate WorkOrder
   *
   * WorkOrder -> HistoryTreeWidget : << signal:destroyed >>
   * WorkOrder <-- HistoryTreeWidget : slot:removeFromHistory
   *
   * deactivate WorkOrder
   * @enduml
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
   *   @history 2017-02-06 Tracie Sucharski - Added methods to set/get whether work order is put on
   *                          the QUndoStack.  If it is NOT put on the stack, it will be greyed out
   *                          in the HistoryTreeWidget and not undo-able.  Todo:  Decide whether
   *                          work orders not on the QUndoStack should appear in the
   *                          HistoryTreeWidget.  Fixes #4598.
   *   @history 2017-04-04 Tracie Sucharski - Renamed the execute method to setupExecution.
   *                          Fixes #4718.
   *   @history 2017-04-04 Tracie Sucharski - Renamed onUndoStack to isUndoable.  Renamed
   *                          setUndoRedo to setUndoable.  Fixes #4722.
   *   @history 2017-04-04 JP Bonn - Updated to new design.  setupExecution() used for preparation.
   *                          No longer separate methods for sync/async - workorder::execute()
   *                          handles both sync/async
   *   @history 2017-04-16 Ian Humphrey - Added enableWorkOrder and disableWorkOrder slots for
   *                           enabling and disabling work orders. Copy constructor now copies
   *                           what's this and tool tip (hover text) state.
   *   @history 2017-05-05 Tracie Sucharski - Added functionality for FileItem types and added
   *                           BundleObservationViewWorkOrder. Fixes #4838, #4839, #4840.
   *   @history 2017-07-24 Cole Neubauer - Created isSavedToHistory() to be able to keep Views from
   *                           being added to the HistoryTree Fixes #4715
   *   @history 2017-07-31 Cole Neubauer - Added a QTMutexLocker to every function that returns a
   *                           member variable function Fixes #5082
   *   @history 2017-08-02 Cole Neubauer - Moved m_status to protected so children can set it
   *                           if a workorder errors Fixes #5026
   *   @history 2017-08-11 Cole Neubauer - Updated documentation for accessor methods and when one
   *                           of these accessors should be used in the workorder template #5113
   *   @history 2017-11-02 Tyler Wilson - Added a virtual setData method for a QString, and
   *                           a private QString object called m_data.  References #4492.
   *   @history 2017-12-05 Christopher Combs - Added support for TemplateEditorWidget and
   *                           TemplateEditViewWorkOrder. Fixes #5168.
   *   @history 2018-06-28 Makayla Shepherd - Removed the ProgressBar cleanup because it was
   *                           causing a seg fault when the ProgressBar was added to the
   *                           HistoryTreeWidget. The HistoryTreeWidget will now clean up the
   *                           ProgressBar. Fixes #5228.
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
      virtual bool isExecutable(Template *currentTemplate);
      virtual bool isExecutable(GuiCameraQsp guiCamera);
      virtual bool isExecutable(FileItemQsp fileItem);
      virtual bool isExecutable(ProjectItem *item);

      void save(QXmlStreamWriter &stream) const;

      virtual void setData(Context);
      virtual void setData(QString data);
      virtual void setData(ImageList *images);
      virtual void setData(ShapeList *shapes);
      virtual void setData(ControlList *controls);
      virtual void setData(Template *currentTemplate);
      virtual void setData(CorrelationMatrix);
      virtual void setData(TargetBodyQsp targetBody);
      virtual void setData(GuiCameraQsp guiCamera);
      virtual void setData(FileItemQsp fileItem);
      virtual void setData(ProjectItem *item);



      void setNext(WorkOrder *nextWorkOrder);
      void setPrevious(WorkOrder *previousWorkOrder);

      QString bestText() const;
      bool isUndoable() const;
      bool isSavedToHistory() const;
      bool isSynchronous() const;
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
      void enableWorkOrder();
      void disableWorkOrder();

      virtual bool setupExecution();

      virtual void execute();

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

      Template *getTemplate();

      TargetBodyQsp targetBody();

      GuiCameraQsp guiCamera();

      FileItemQsp fileItem();

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
      virtual void postExecution();
      virtual void undoExecution();
      virtual void postUndoExecution();

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
      void executionFinished();
      void clearImageList();
      void clearShapeList();
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

    protected:
      /**
       * Set the workorder to be undoable/redoable
       * This is defaulted to true - his will allow the workorder to be redone.  Note
       * the workorder undoExecution() method must be implemented.  This will result on the
       * workorder being placed on the QUndoStack and being displayed in the history
       * as being undoable. If set to false, the work order will not be put on the
       * QUndoStack and the workorder will not be able to be undone.
       */
      bool m_isUndoable;

       /**
        * This is defaulted to true. If true, the work order will be executed on the GUI
        * thread synchronously. If false, then the work order will be queued for execution
        * on a non-GUI thread and will not block the GUI.
        */
       bool m_isSynchronous;

       /**
        * Set the work order to be shown in the HistoryTreeWidget.
        * This is defaulted to true. If true the work order will be shown in the
        * HistoryTreeWidget if false it will not be shown.
        */
       bool m_isSavedToHistory;

       WorkOrderStatus m_status;

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
      QString m_data;
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
       * A QSharedPointer to the Template (A Template object but encapsulated within a Gui
       * framework.
       */
      Template *m_template;


      /**
       * A QSharedPointer to the TargetBody (A Target object but encapsulated within a Gui
       * framework.
       */
      TargetBodyQsp m_targetBody;


      /**
       * A QSharedPointer to the FileItem
       */
      FileItemQsp m_fileItem;


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
       * This is the date/time that setupExecution() was called.
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
       * A QElapsedTimer object holding the excecution time of the WorkOrder.
       */
      QElapsedTimer *m_elapsedTimer;

      /**
       * @brief The seconds that have elapsed since the WorkOrder started executing.
       */
      double m_secondsElapsed;
  };
}

//! This allows WorkOrder *'s to be stored in a QVariant.
Q_DECLARE_METATYPE(Isis::WorkOrder *);

#endif
