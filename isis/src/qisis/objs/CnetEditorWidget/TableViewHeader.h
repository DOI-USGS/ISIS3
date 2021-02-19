#ifndef TableViewHeader_H
#define TableViewHeader_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QWidget>

class QMouseEvent;
class QPainter;
class QPaintEvent;
class QSize;
class QStringList;
template<typename T> class QList;


namespace Isis {
  class AbstractTableModel;
  class TableColumnList;

  /**
    * @author ????-??-?? Unknown
    *
    * @internal
    *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
    *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
    *   @history 2017-08-08 Makayla Shepherd - Updated documentation.
    */
  class TableViewHeader : public QWidget {

      Q_OBJECT

    public:
      explicit TableViewHeader(AbstractTableModel *someModel);
      virtual ~TableViewHeader();
      virtual void setColumns(TableColumnList *);
      QSize minimumSizeHint() const;
      QSize sizeHint() const;

      void setModel(AbstractTableModel *someModel);


    signals:
      void columnResized(bool lastColumn);
      void requestedGlobalSelection(bool select);
      void requestedColumnSelection(int columnNum, bool select);
      void sortingEnabled(bool);


    public slots:
      void handleFilterCountsChanged(int visibleTopLevelItemCount,
          int topLevelItemCount);
      void updateHeaderOffset(int);
      void updateFilterProgress(int newProgress);
      void updateFilterProgressRange(int min, int max);
      void updateRebuildProgress(int newProgress);
      void updateRebuildProgressRange(int min, int max);
      void updateSortProgress(int newProgress);
      void updateSortProgressRange(int min, int max);


    protected:
      void mouseMoveEvent(QMouseEvent *event);
      void mousePressEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void paintEvent(QPaintEvent *event);


    private: // methods
      TableViewHeader(const TableViewHeader &other);
      TableViewHeader &operator=(TableViewHeader other);
      void nullify();
      QRect getColumnRect(int column) const;
      int getMousedColumn(QPoint mousePos);
      int getMousedColumnEdge(QPoint mousePos);
      bool mouseAtResizableColumnEdge(QPoint mousePos);
      void paintHeader(QPainter *painter, int rowheight);
      void paintProgress(QPainter *painter, const QRect &rect, int min,
          int max, int value, bool over100);
      QRect getSortingPriorityRect(int visColIndex);
      QRect getSortingArrowRect(int visColIndex);


    private: // data
      TableColumnList *m_columns;
      int m_horizontalOffset;
      int m_filterProgress;
      int m_filterProgressMin;
      int m_filterProgressMax;
      int m_rebuildProgress;
      int m_rebuildProgressMin;
      int m_rebuildProgressMax;
      int m_sortProgress;
      int m_sortProgressMin;
      int m_sortProgressMax;

      int m_visibleCount;
      int m_totalCount;

      int m_clickedColumnEdge;
      int m_clickedColumn;

      static int const SORT_ARROW_MARGIN = 10;
      int ARROW_HEIGHT;
      int ARROW_WIDTH;

      AbstractTableModel *m_model;
  };
}


#endif
