#ifndef HistoryTreeWidget_H
#define HistoryTreeWidget_H

#include <QTreeWidget>
#include <QMutex>


class QResizeEvent;
class QUndoCommand;

namespace Isis {
  class Project;
  class WorkOrder;

  /**
   * @brief History Widget for ipce
   *
   * This widget shows the history of work orders performed on the project.
   *
   * @author 2012-07-23 Steven Lambright
   *
   * @internal
   *   @history 2012-07-23 Steven Lambright - Added/fixed documentation, improved implementation of
   *                           addToHistory to look for alternative text/not display blank items.
   *   @history 2012-07-23 Steven Lambright and Stuart Sides - Added display of undo stack
   *   @history 2012-07-27 Kimberly Oyama and Steven Lambright - Added resizeEvent() and improved
   *                           implementation of refit().
   *   @history 2012-07-31 Kimberly Oyama - Added comments to some of the methods.
   *   @history 2017-02-06 Tracie Sucharski - Work orders that are not on the QUndoStack are greyed
   *                           out and italicized.
   *   @history 2017-04-05 Tracie Sucharski - For the last change, method name was changed from
   *                           onUndoStack to isUndoable.
   *   @history 2017-07-24 Cole Neubauer - Added check in addToHistory() to check if a WorkOrder
   *                           should be added to the HistoryTree Fixes #4715
   *   @history 2017-08-10 Tyler Wilson - Changed some code in addToHistory function which
   *                            was causing a segfault.  This is a bandaid fix which
   *                            addresses the immediate problem, but will have to be tackled
   *                            in a future ticket.  Fixes #5096.  References #4492.
   *   @history 2017-08-11 Cole Neubauer - Added some checks to avoid segfaults Fixes #5064
   *   @history 2017-11-08 Tyler Wilson - Reverted the code change in #5096 to restore the 
   *                            ProgressBar, and changed code in OpenRecentProjectsWorkOrder.cpp
   *                            to prevent the segfault which #5096 was addressing.  Fixes #5149.
   *   @history 2018-04-07 Tracie Sucharski - Added method to force a history entry using a string
   *                            rather than a WorkOrder.  This should be a temporary method until
   *                            saving a control is turned into a WorkOrder.  This was done for the
   *                            alpha release simply to notify the user that the control was saved.
   *                            However, this history entry is not saved/restored to a project.
   *
   */
  class HistoryTreeWidget : public QTreeWidget {
      Q_OBJECT
    public:
      HistoryTreeWidget(Project *project, QWidget *parent = 0);
      virtual ~HistoryTreeWidget();

      void addToHistory(QString historyEntry);

    protected:
      int sizeHintForColumn(int column) const;

    private:
      void refit();
      void updateStatus(QTreeWidgetItem *);
      QMutex m_mutex;

    private slots:
      void addToHistory(WorkOrder *);
      void updateProgressWidgets();
      void markNotUndone(QTreeWidgetItem *);
      void markUndone(QTreeWidgetItem *);
      void handleUndoIndexChanged(int);
      void removeFromHistory(QObject *);
      void showHistory();
      void updateStatus(WorkOrder *);


      QTreeWidgetItem *undoCommandToTreeItem(const QUndoCommand *);

    private:
      Q_DISABLE_COPY(HistoryTreeWidget);

      Project *m_project; //! The project associated with the history.
  };
}

#endif
