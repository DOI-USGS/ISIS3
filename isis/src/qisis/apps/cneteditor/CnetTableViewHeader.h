#ifndef CnetTableViewHeader_H
#define CnetTableViewHeader_H

#include <QWidget>

class QMouseEvent;
class QPainter;
class QPaintEvent;
class QSize;
class QStringList;
template<typename T> class QList;


namespace Isis
{
  class AbstractCnetTableModel;
  class CnetTableColumnList;

  class CnetTableViewHeader : public QWidget
  {

      Q_OBJECT

    public:
      explicit CnetTableViewHeader(AbstractCnetTableModel * someModel);
      virtual ~CnetTableViewHeader();
      virtual void setColumns(CnetTableColumnList *);
      QSize minimumSizeHint() const;
      QSize sizeHint();
      
      void setModel(AbstractCnetTableModel * someModel);


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


    protected:
      void mouseMoveEvent(QMouseEvent * event);
      void mousePressEvent(QMouseEvent * event);
      void mouseReleaseEvent(QMouseEvent * event);
      void paintEvent(QPaintEvent * event);


    private: // methods
      CnetTableViewHeader(const CnetTableViewHeader & other);
      CnetTableViewHeader & operator=(CnetTableViewHeader other);
      void nullify();
      QRect getColumnRect(int column) const;
      int getMousedColumn(QPoint mousePos);
      int getMousedColumnEdge(QPoint mousePos);
      bool mouseAtResizableColumnEdge(QPoint mousePos);
      void paintHeader(QPainter * painter, int rowheight);
      void paintProgress(QPainter * painter, const QRect & rect, int min,
          int max, int value);
      QRect getSortingPriorityRect(int visColIndex);
      QRect getSortingArrowRect(int visColIndex);


    private: // data
      CnetTableColumnList * columns;
      int horizontalOffset;
      int filterProgress;
      int filterProgressMin;
      int filterProgressMax;
      int rebuildProgress;
      int rebuildProgressMin;
      int rebuildProgressMax;

      int visibleCount;
      int totalCount;

      int clickedColumnEdge;
      int clickedColumn;
      
      static int const SORT_ARROW_MARGIN = 10;
      int ARROW_HEIGHT;
      int ARROW_WIDTH;
      
      AbstractCnetTableModel * model;
  };
}


#endif
