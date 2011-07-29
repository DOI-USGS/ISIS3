#ifndef CnetTableViewContent_H
#define CnetTableViewContent_H

#include <QAbstractScrollArea>


class QEvent;
class QKeyEvent;
template< typename T > class QList;
class QMouseEvent;
class QPoint;
class QResizeEvent;


namespace Isis
{
  class AbstractTreeItem;
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
      void selectionChanged();
      void horizontalScrollBarValueChanged(int);

    public slots:
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
      void clearColumnSelection();
      int getColumnFromScreenX(int screenX) const;
      int getRowFromScreenY(int screenY) const;
      bool isRowValid(int rowNum) const;
      bool isColumnValid(int colNum) const;
      bool isCellEditable(int, int) const;
      void paintRow(QPainter *, int, QPoint, QPoint);
      void updateActiveCell(QPoint);
      void updateHoveredCell(QPoint, bool);
      void updateColumnGroupSelection(AbstractTreeItem *);
      void updateRowGroupSelection(int lastRow);


    private slots:
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


    private:
      static const int ITEM_PADDING = 7;
      static const int ITEM_INDENTATION = 3;
  };
}

#endif

