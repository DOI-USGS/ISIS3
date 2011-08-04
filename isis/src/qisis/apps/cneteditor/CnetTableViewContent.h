#ifndef CnetTableViewContent_H
#define CnetTableViewContent_H

#include <QAbstractScrollArea>


class QAction;
class QEvent;
class QKeyEvent;
template< typename T > class QList;
class QMenu;
class QMouseEvent;
class QPoint;
class QResizeEvent;


namespace Isis
{
  class AbstractTreeItem;
  class CnetTableColumn;
  class CnetTableColumnList;
  class CnetTableView;
  class AbstractCnetTableModel;

  class CnetTableViewContent : public QAbstractScrollArea
  {
      Q_OBJECT

    public:
      CnetTableViewContent(CnetTableColumnList * cols);
      virtual ~CnetTableViewContent();
      QSize minimumSizeHint() const;
      QSize sizeHint();
      AbstractCnetTableModel * getModel();
      void setModel(AbstractCnetTableModel * someModel);


    signals:
      void rebuildModels(QList<AbstractTreeItem *>);
      void modelDataChanged();
      void tableSelectionChanged();
      void tableSelectionChanged(QList< AbstractTreeItem * >);
      void horizontalScrollBarValueChanged(int);


    public slots:
      void scrollTo(QList< AbstractTreeItem * >);
      void scrollTo(AbstractTreeItem *);
      void refresh();
      void updateHorizontalScrollBar(bool scrollRight = false);
      

    protected:
      bool eventFilter(QObject * target, QEvent * event);
      void keyPressEvent(QKeyEvent * event);
      void leaveEvent(QEvent * event);
      void mouseDoubleClickEvent(QMouseEvent * event);
      void mouseMoveEvent(QMouseEvent * event);
      void mousePressEvent(QMouseEvent * event);
      void mouseReleaseEvent(QMouseEvent * event);
      void paintEvent(QPaintEvent * event);
      void resizeEvent(QResizeEvent * event);
      void scrollContentsBy(int dx, int dy);


    private:
      CnetTableViewContent(const CnetTableViewContent & other);
      CnetTableViewContent & operator=(const CnetTableViewContent & other);


    private:
      void nullify();
      void cellDataChanged(CnetTableColumn const * col);
      void clearActiveCell();
      void clearColumnSelection();
      void copyCellSelection(bool);
      void createActions();
      int getColumnFromScreenX(int screenX) const;
      int getRowFromScreenY(int screenY) const;
      bool hasActiveCell() const;
      bool hasRowSelection() const;
      bool mouseInCellSelection(QPoint) const;
      bool isMouseInRowSelection(QPoint) const;
      bool isRowValid(int rowNum) const;
      bool isColumnValid(int colNum) const;
      bool isCellEditable(int, int) const;
      bool isDataColumn(int) const;
      void paintRow(QPainter *, int, QPoint, QPoint);
      void updateActiveCell(QPoint);
      void updateHoveredCell(QPoint, bool);
      void updateColumnGroupSelection(AbstractTreeItem *);
      QList< AbstractTreeItem * > updateRowGroupSelection(int lastRow);


    private slots:
      void copySelection();
      void copyAll();
      void deleteSelectedRows();
      void updateItemList();
      void showContextMenu(QPoint);


    private:
      CnetTableView * parentView;
      AbstractCnetTableModel * model;
      QList< AbstractTreeItem * > * items;
      CnetTableColumnList * columns;

      QPair<AbstractTreeItem *, int> * activeCell;

      /**
       * Stores a list of the rows that have their active column cells
       *   selected.
       */
      QList<AbstractTreeItem *> * rowsWithActiveColumnSelected;

      QWidget * editWidget;

      /**
       * This is the last row that was selected by either a control-click or
       * normal click.
       */
      AbstractTreeItem * lastDirectlySelectedRow;
      QList<AbstractTreeItem *> * lastShiftSelection;
      QPoint * mousePressPos;
      int rowHeight;

      /**
       * This action applies (copies) the contents of the active cell to the
       * current selection.
       */
      QAction * applyToSelectionAct;

      /**
       * This action applies (copies) the contents of the active cell to all of
       * the cells in the active cell's column.
       */
      QAction * applyToAllAct;

      /**
       * This action deletes the selected rows.
       */
      QAction * deleteSelectedRowsAct;


    private:
      static const int ITEM_PADDING = 7;
      static const int ITEM_INDENTATION = 3;
  };
}

#endif

