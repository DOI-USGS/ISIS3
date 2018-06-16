#include "HistoryTreeWidget.h"

#include <QDateTime>
#include <QDebug>

#include "ProgressBar.h"
#include "Project.h"
#include "WorkOrder.h"

namespace Isis {
  /**
   * Construct a history tree widget
   *
   * @param project The project to show history for
   * @param parent The Qt-relationship parent
   */
  HistoryTreeWidget::HistoryTreeWidget(Project *project, QWidget *parent) : QTreeWidget(parent) {
    m_project = project;

    QStringList headers;
    headers.append("Operation");
    headers.append("Progress");
    headers.append("Time Executed");

    setHeaderLabels(headers);

    connect(m_project, SIGNAL(workOrderStarting(WorkOrder *)),
            this, SLOT(addToHistory(WorkOrder *)));
    connect(m_project, SIGNAL(projectLoaded(Project *)),
            this, SLOT(showHistory()));
    connect(m_project->undoStack(), SIGNAL(indexChanged(int)),
            this, SLOT(handleUndoIndexChanged(int)));

    showHistory();

    refit();
  }


  /**
   * Clean up allocated memory
   */
  HistoryTreeWidget::~HistoryTreeWidget() {
    m_project = NULL;
  }


  /**
   * Get the preferred size of a given column. This is used by resizeColumnToContents().
   *
   * The work order name column prefers space not taken by other columns.
   * The progress column prefers 200.
   * The date column prefers a little more than the text size of a date.
   *
   * @return The preferred width of the given column
   */
  int HistoryTreeWidget::sizeHintForColumn(int column) const {
    int result = -1;

    QFontMetrics metrics(invisibleRootItem()->font(1));
    int dateTimeColumnWidth = qRound(metrics.width(QDateTime::currentDateTime().toString()) * 1.10);

    int progressBarColumnWidth = 200;

    // The -12 is a guesstimate for not having a horizontal scroll bar. I'm not sure how to
    //   calculate this number correctly. Consequences of it being wrong are minimal. -SL
    int operationColumnWidth = width() - dateTimeColumnWidth - progressBarColumnWidth - 12;

    if (operationColumnWidth > 0) {
      if (column == 0)
        result = operationColumnWidth;
      else if (column == 1)
        result = progressBarColumnWidth;
      else if (column == 2)
        result = dateTimeColumnWidth;
    }

    return result;
  }


  /**
   * This resizes the columns to an okay width for viewing all of the data cleanly. This method
   *   depends on sizeHintForColumn() for good column sizes.
   */
  void HistoryTreeWidget::refit() {
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
  }


  void HistoryTreeWidget::updateStatus(QTreeWidgetItem *treeItem) {
    WorkOrder *workOrder = treeItem->data(0, Qt::UserRole).value<WorkOrder *>();

    if (workOrder)
      updateStatus(workOrder);
  }


  /**
   * Add a single work order to the display. This uses the QUndoCommand text (if it's blank, it uses
   *   the QAction text). If there is no text, this does nothing.
   *
   * @param workOrder The work order to display the history for
   */
  void HistoryTreeWidget::addToHistory(WorkOrder *workOrder) {

    QString data = workOrder->bestText();


    connect(workOrder, SIGNAL(destroyed(QObject *)),
                this, SLOT(removeFromHistory(QObject *)));

    QStringList columnData;
    columnData.append(data);
    columnData.append("");
    columnData.append(workOrder->executionTime().toString());

    QTreeWidgetItem *newItem = new QTreeWidgetItem(columnData);
    newItem->setData(0, Qt::UserRole, qVariantFromValue(workOrder));

    // Do font for save work orders or work orders not on QUndoStack
    if (workOrder->createsCleanState() || !workOrder->isUndoable()) {
      QFont saveFont = newItem->font(0);
      saveFont.setBold(true);
      saveFont.setItalic(true);
      newItem->setFont(0, saveFont);
      newItem->setForeground(0, Qt::gray);
      }

      // Do font for progress text
      QFont progressFont = newItem->font(1);
      progressFont.setItalic(true);
      newItem->setFont(1, progressFont);
      newItem->setForeground(1, Qt::gray);

      this->insertTopLevelItem(0, newItem);
//      invisibleRootItem()->addChild(newItem);

      connect(workOrder, SIGNAL(statusChanged(WorkOrder *)),
                this, SLOT(updateStatus(WorkOrder *)));
      connect(workOrder, SIGNAL(creatingProgress(WorkOrder *)),
                this, SLOT(updateProgressWidgets()));
      connect(workOrder, SIGNAL(deletingProgress(WorkOrder *)),
                this, SLOT(updateProgressWidgets()));


      //Sometimes the pointer returned by this call is 0 (hence the check).
      //So we are not creating a progress bar for every work order which would
      //include those that do not need it.

      if(workOrder->progressBar() )  {
        setItemWidget(newItem, 1, new ProgressBar);
//        this->setItemWidget(newItem, 1, workOrder->progressBar() );
      }
      scrollToItem(newItem);
      refit();
  }


  /**
   * Add a non-workorder history to the display.
   *
   * @param (QString) historyEntry The string displayed in the history tree
   */
  void HistoryTreeWidget::addToHistory(QString historyEntry) {

    QString data = historyEntry;

    QStringList columnData;
    columnData.append(data);

    QTreeWidgetItem *newItem = new QTreeWidgetItem(columnData);


    // Do font for progress text
    QFont progressFont = newItem->font(1);
    progressFont.setItalic(true);
    newItem->setFont(1, progressFont);
    newItem->setForeground(1, Qt::gray);

    this->insertTopLevelItem(0, newItem);
//      invisibleRootItem()->addChild(newItem);

    //Sometimes the pointer returned by this call is 0 (hence the check).
    //So we are not creating a progress bar for every work order which would
    //include those that do not need it.

//    if(workOrder->progressBar() )  {
//      setItemWidget(newItem, 1, new ProgressBar);
////        this->setItemWidget(newItem, 1, workOrder->progressBar() );
//    }
    scrollToItem(newItem);
    refit();
  }


  /**
   * We need to manually manage these progress widgets because QTreeWidget does a poor job of it.
   *   This should be called when the progress bar instances have changed (new progress, lost a
   *   progress, etc...). This is not necessary when the progress values have changed.
   *
   * Failing to call this method results in seg faults when other events occur - such as a
   *   resize event.
   */
  void HistoryTreeWidget::updateProgressWidgets() {
    if( !m_project->clearing() ){
      for (int i = 0; i < invisibleRootItem()->childCount(); i++) {
        QTreeWidgetItem *item = invisibleRootItem()->child(i);
        if (item->data(0, Qt::UserRole).toString() != "") {
          WorkOrder *workOrder = item->data(0, Qt::UserRole).value<WorkOrder *>();
          if (workOrder && itemWidget(item, 1) != workOrder->progressBar()) {
            setItemWidget(item, 1, workOrder->progressBar());
          }
        }
      }
    }
  }


  /**
   * Display the item as not an item that has been undone - it's working or done. This colors it
   *   black and sets the text to completed (only visible if there is no progress bar).
   *
   * @param treeItem The given GUI tree item/row to color appropriately.
   */
  void HistoryTreeWidget::markNotUndone(QTreeWidgetItem *treeItem) {
    if (treeItem) {
      treeItem->setForeground(0, Qt::black);
      updateStatus(treeItem);
    }
  }


  /**
   * Display the item as an item that has been undone. This colors it
   *   gray and sets the text to Undone.
   *
   * @param treeItem The given GUI tree item/row to color appropriately.
   */
  void HistoryTreeWidget::markUndone(QTreeWidgetItem *treeItem) {
    if (treeItem) {
      treeItem->setForeground(0, Qt::gray);
      updateStatus(treeItem);
    }
  }


  /**
   * The project's undo stack has changed. Display the changed states appropriately.
   */
  void HistoryTreeWidget::handleUndoIndexChanged(int newIndex) {
    QTreeWidgetItem *prevItem = undoCommandToTreeItem(
        m_project->undoStack()->command(newIndex - 1));
    markNotUndone(prevItem);

    QTreeWidgetItem *curItem = undoCommandToTreeItem(m_project->undoStack()->command(newIndex));
    markUndone(curItem);

    markUndone(undoCommandToTreeItem(m_project->undoStack()->command(newIndex + 1)));

    scrollToItem(prevItem);
    scrollToItem(curItem);
  }


  /**
   * A work order was lost... compensate by removing it from the tree.
   */
  void HistoryTreeWidget::removeFromHistory(QObject *deletedObject) {
    QTreeWidgetItem *itemToRemove = undoCommandToTreeItem( (QUndoCommand *)((WorkOrder *)deletedObject));

    if (itemToRemove) {

      int indexToDelete = invisibleRootItem()->indexOfChild(itemToRemove);
      if (indexToDelete < invisibleRootItem()->childCount()) {

        // Clear progress bar widget
        setItemWidget(invisibleRootItem()->child(indexToDelete), 1, NULL);

        // Take & delete the tree item
        delete invisibleRootItem()->takeChild(indexToDelete);
      }
    }
  }


  /**
   * Get the QTreeWidgetItem associated with the given undo command (work order). Returns NULL if
   *   none found or given NULL.
   *
   * @param undoCommand A work order
   * @return The QTreeWidgetItem that represents (is associated with) the undoCommand
   */
  QTreeWidgetItem *HistoryTreeWidget::undoCommandToTreeItem(const QUndoCommand *undoCommand) {
    QTreeWidgetItem *result = NULL;
    if (undoCommand) {
      for (int i = invisibleRootItem()->childCount() - 1; !result && i >= 0; i--) {
        QTreeWidgetItem *item = invisibleRootItem()->child(i);
        if (item->data(0, Qt::UserRole).toString() != "" ) {
          WorkOrder *workOrder = item->data(0, Qt::UserRole).value<WorkOrder *>();

          if (undoCommand == workOrder)
            result = item;
        }
      }
    }
    return result;
  }


  /**
   * This resets the tree widget and re-initializes
   *
   * @see addToHistory()
   *
   */
  void HistoryTreeWidget::showHistory() {
    foreach (QTreeWidgetItem *item, invisibleRootItem()->takeChildren()) {
      delete item;
    }

    foreach (WorkOrder *workOrder, m_project->workOrderHistory()) {
      addToHistory(workOrder);
    }
  }


  void HistoryTreeWidget::updateStatus(WorkOrder *workOrder) {
    if (undoCommandToTreeItem(workOrder)) {
      undoCommandToTreeItem(workOrder)->setText(1, workOrder->statusText());
    }

  }
}
