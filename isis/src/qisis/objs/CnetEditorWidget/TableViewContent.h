#ifndef TableViewContent_H
#define TableViewContent_H

#include <QAbstractScrollArea>

#include <QPointer>


class QAction;
class QEvent;
class QKeyEvent;
template< typename T > class QList;
class QMenu;
class QMouseEvent;
class QPoint;
class QResizeEvent;


namespace Isis {
  namespace CnetViz {
    class AbstractTableModel;
    class AbstractTreeItem;
    class TableColumn;
    class TableColumnList;
    class TableView;

    /**
     * @author ????-??-?? Unknown
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class TableViewContent : public QAbstractScrollArea {
        Q_OBJECT

      public:
        TableViewContent(AbstractTableModel *someModel);
        virtual ~TableViewContent();
        QSize minimumSizeHint() const;
        QSize sizeHint();
        AbstractTableModel *getModel();
        //       void setModel(AbstractTableModel * someModel);


      signals:
        void rebuildModels(QList< CnetViz::AbstractTreeItem * >);
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
        bool eventFilter(QObject *target, QEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void leaveEvent(QEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);
        void resizeEvent(QResizeEvent *event);
        void scrollContentsBy(int dx, int dy);


      private:
        TableViewContent(const TableViewContent &other);
        TableViewContent &operator=(const TableViewContent &other);


      private:
        void nullify();
        void cellDataChanged(TableColumn const *col);
        void clearActiveCell();
        void clearColumnSelection();
        void copyCellSelection(bool);
        void createActions();
        void selectAllRows();
        int getColumnFromScreenX(int screenX) const;
        int getRowFromScreenY(int screenY) const;
        bool hasActiveCell() const;
        bool hasRowSelection() const;
        bool mouseInCellSelection(QPoint) const;
        bool mouseInRowSelection(QPoint) const;
        bool rowIsValid(int rowNum) const;
        bool columnIsValid(int colNum) const;
        bool cellIsEditable(int, int) const;
        bool isDataColumn(int) const;
        void paintRow(QPainter *, int, QPoint, QPoint);
        void updateActiveCell(QPoint);
        void updateHoveredCell(QPoint, bool);
        void updateColumnGroupSelection(AbstractTreeItem *);
        QList< AbstractTreeItem * > updateRowGroupSelection(int lastRow);
        void finishEditing();
        void moveActiveCellDown();
        void moveActiveCellUp();
        void moveActiveCellLeft();
        void moveActiveCellRight();


      private slots:
        void copySelection();
        void copyAll();
        void deleteSelectedRows();
        void updateItemList();
        void showContextMenu(QPoint);


      private:
        TableView *m_parentView;
        AbstractTableModel *m_model;
        QList< QPointer<AbstractTreeItem> > * m_items;
        TableColumnList *m_columns;

        QPair< AbstractTreeItem *, int > * m_activeCell;

        QPair< AbstractTreeItem *, int > * m_lastShiftArrowSelectedCell;
        bool m_lastShiftArrowDirectionUp;

        /**
        * Stores a list of the rows that have their active column cells
        *   selected.
        */
        QList< AbstractTreeItem * > * rowsWithActiveColumnSelected;

        QWidget *m_editWidget;

        /**
        * This is the last row that was selected by either a control-click or
        * normal click.
        */
        AbstractTreeItem *m_lastDirectlySelectedRow;
        QList< AbstractTreeItem * > * m_lastShiftSelection;
        QPoint *m_mousePressPos;
        int m_rowHeight;

        /**
        * This action applies (copies) the contents of the active cell to the
        * current selection.
        */
        QAction *m_applyToSelectionAct;

        /**
        * This action applies (copies) the contents of the active cell to all of
        * the cells in the active cell's column.
        */
        QAction *m_applyToAllAct;

        /**
        * This action deletes the selected rows.
        */
        QAction *m_deleteSelectedRowsAct;


      private:
        static const int ITEM_PADDING = 7;
        static const int ITEM_INDENTATION = 3;
    };
  }
}

#endif

