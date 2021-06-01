#ifndef TableViewContent_H
#define TableViewContent_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QAbstractScrollArea>

#include <QPoint>
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
  class AbstractTreeItem;
  class AbstractTableModel;
  class ControlPoint;
  class TableColumn;
  class TableColumnList;
  class TableView;

  /**
    * @author ????-??-?? Unknown
    *
    * @internal
    *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
    *   @history 2017-05-18 Tracie Sucharski - Added a new QAction showing on the context menu
    *                           allowing a control point to be edited in IPCE.  Added signal to
    *                           indicate the control point chosen from either the point table or
    *                           the measure table.  If the point was chosen from the measure table,
    *                           the serial number of the measure is also passed. This was added for
    *                           IPCE, for the interaction with other views.
    *   @history 2017-07-18 Christopher Combs - Fixed bug in which trying to edit a selected row
    *                           would cause a segfault from m_activeCell being null. Fixes #4958.
    *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
    *   @history 2017-08-08 Makayla Shepherd - Fixed a seg fault in ipce that occurs when
    *                           attempting to edit a control point when there is not an active
    *                           control network. Fixes #5048.
    *   @history 2017-09-20 Ian Humphrey - Modified showContextMenu so single line if statements
    *                           have braces. This prevents a misleading-indentation warning from
    *                           occuring during Fedora25 (c++14) builds. No functionality has been
    *                           changed. References #4809.
    *   @history 2018-07-17 Kaitlyn Lee - Modified showContextMenu() to enable
                                m_deleteSelectedRowsAct regardless if an active control is set.
    */
  class TableViewContent : public QAbstractScrollArea {
      Q_OBJECT

    public:
      TableViewContent(AbstractTableModel *someModel);
      virtual ~TableViewContent();
      QSize minimumSizeHint() const;
      QSize sizeHint() const;
      AbstractTableModel *getModel();
      //       void setModel(AbstractTableModel * someModel);
      void setActiveControlNet(bool activeNet);


    signals:
      void rebuildModels(QList< AbstractTreeItem * >);
      void modelDataChanged();
      void tableSelectionChanged();
      void tableSelectionChanged(QList< AbstractTreeItem * >);
      void horizontalScrollBarValueChanged(int);

      void editControlPoint(ControlPoint *controlPoint, QString serialNumber);


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
      void editControlPoint();
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

      /**
      * This action edits selected control point or if measure selected, edit parent control pt
      */
      QAction *m_editControlPointAct;

      bool m_activeControlNet;


    private:
      static const int ITEM_PADDING = 7;
      static const int ITEM_INDENTATION = 3;
  };
}

#endif
