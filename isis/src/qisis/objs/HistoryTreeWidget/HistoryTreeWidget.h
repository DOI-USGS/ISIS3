#ifndef HistoryTreeWidget_H
#define HistoryTreeWidget_H

#include <QTreeWidget>

class QResizeEvent;
class QUndoCommand;

namespace Isis {
  class Project;
  class WorkOrder;

  /**
   * @brief History Widget for cnetsuite
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
   *  
   */
  class HistoryTreeWidget : public QTreeWidget {
      Q_OBJECT
    public:
      HistoryTreeWidget(Project *project, QWidget *parent = 0);
      virtual ~HistoryTreeWidget();

    protected:
      int sizeHintForColumn(int column) const;

    private:
      void refit();
      void updateStatus(QTreeWidgetItem *);

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
