#include "IsisDebug.h"

#include "CnetTableViewHeader.h"

#include <iostream>

#include <QAbstractItemModel>
#include <QFontMetrics>
#include <QLabel>
#include <QLinearGradient>
#include <QLocale>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QString>
#include <QVBoxLayout>

#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"
#include "CnetTableView.h"
#include "TreeModel.h"

using std::cerr;


namespace Isis
{

  CnetTableViewHeader::CnetTableViewHeader(CnetTableColumnList * cols)
  {
    nullify();

    columns = cols;

    horizontalOffset = 0;
    filterProgress = 0;
    filterProgressMin = 0;
    filterProgressMax = 0;
    rebuildProgress = 0;
    rebuildProgressMin = 0;
    rebuildProgressMax = 0;
    visibleCount = -1;
    totalCount = -1;

    clickedColumnEdge = -1;

    setMouseTracking(true);
  }


  CnetTableViewHeader::~CnetTableViewHeader()
  {
    columns = NULL;
  }


  QSize CnetTableViewHeader::minimumSizeHint() const
  {
    return QSize(0, QFontMetrics(font()).height() + 6);
    /*QFontMetrics(font()).width(text->join("")) + 15,
        QFontMetrics(font()).height() + 6);*/
  }


  void CnetTableViewHeader::handleFilterCountsChanged(
    int visibleTopLevelItemCount, int topLevelItemCount)
  {
    visibleCount = visibleTopLevelItemCount;
    totalCount = topLevelItemCount;

    if (visibleCount >= 0)
    {
      CnetTableColumnList visibleCols = columns->getVisibleColumns();
      foreach(CnetTableColumn * col, visibleCols)
      {
        if (col->getTitle().isEmpty())
          col->setWidth(QFontMetrics(font()).width(
              QString::number(visibleCount)) + 22);
      }
    }

    updateGeometry();
    update();
  }


  void CnetTableViewHeader::updateHeaderOffset(int newOffset)
  {
    horizontalOffset = newOffset;
    update();
  }


  void CnetTableViewHeader::mouseMoveEvent(QMouseEvent * event)
  {
    QPoint mousePos = event->pos();

    if (clickedColumnEdge >= 0)
    {
      // edge == column that we want to resize
      QRect columnToResizeRect(getColumnRect(clickedColumnEdge));
      columnToResizeRect.setRight(mousePos.x());

      int newWidth = 1;
      if (columnToResizeRect.width() > 1)
      {
        newWidth = columnToResizeRect.width();
      }

      columns->getVisibleColumns()[clickedColumnEdge]->setWidth(newWidth);
    }

    if (mouseAtResizableColumnEdge(mousePos))
    {
      setCursor(Qt::SizeHorCursor);
    }
    else
    {
      setCursor(Qt::ArrowCursor);
    }

    update();
  }


  void CnetTableViewHeader::mousePressEvent(QMouseEvent * event)
  {
    QPoint mousePos = event->pos();

    if (event->buttons() == Qt::LeftButton)
    {
      clickedColumnEdge = getMousedColumnEdge(mousePos);
      int columnNum = getMousedColumn(mousePos);
      if (clickedColumnEdge == -1 && columnNum != -1)
      {
        // The click wasn't on a column edge.
        if (columns->getVisibleColumns().at(columnNum)->getTitle().isEmpty())
          emit requestedGlobalSelection(true);
        else
          emit requestedColumnSelection(columnNum, true);
      }
    }
  }


  void CnetTableViewHeader::mouseReleaseEvent(QMouseEvent * event)
  {
    bool wasLastCol =
      clickedColumnEdge >= columns->getVisibleColumns().size() - 2;
    if (clickedColumnEdge != -1)
      emit columnResized(wasLastCol);

    clickedColumnEdge = -1;
    update();
  }


  void CnetTableViewHeader::paintEvent(QPaintEvent * event)
  {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing);
    paintHeader(&painter, height());
//    painter.drawRect(0, 0, width(), height());
    painter.end();
  }


  void CnetTableViewHeader::nullify()
  {
    columns = NULL;
  }


  QRect CnetTableViewHeader::getColumnRect(int column) const
  {
    QRect colRect;

    CnetTableColumnList visibleCols = columns->getVisibleColumns();

    if (column < visibleCols.size() && column >= 0)
    {
      int indent = 1;

      for (int i = 0; i < column; i++)
        indent += visibleCols.at(i)->getWidth() - 1;

      colRect = QRect(indent - horizontalOffset, 0,
          visibleCols.at(column)->getWidth(), height());
    }

    return colRect;
  }


  int  CnetTableViewHeader::getMousedColumn(QPoint mousePos)
  {
    int mousedColumn = -1;

    for (int i = 0;
        i < columns->getVisibleColumns().size() && mousedColumn < 0;
        i++)
    {
      QRect columnRect(getColumnRect(i));
      if (columnRect.contains(mousePos))
        mousedColumn = i;
    }

    return mousedColumn;
  }


  int CnetTableViewHeader::getMousedColumnEdge(QPoint mousePos)
  {
    int edge = -1;

    if (mouseAtResizableColumnEdge(mousePos))
    {
      int mousedColumn = getMousedColumn(mousePos);

      QRect columnRect(getColumnRect(mousedColumn));

      // mouseAtResizableColumnEdge can't be on left of first so this won't return -1
      //   resulting from this particular logic.
      if (mousePos.x() - columnRect.left() < CnetTableColumn::EDGE_WIDTH)
        edge = mousedColumn - 1;
      else
        edge = mousedColumn;
    }

    return edge;
  }


  bool CnetTableViewHeader::mouseAtResizableColumnEdge(QPoint mousePos)
  {
    int columnNum = getMousedColumn(mousePos);

    QRect columnRect(getColumnRect(columnNum));

    bool isAtColumnEdge = false;

    if (!columnRect.isNull())
    {
      bool isOnLeft =
        mousePos.x() - columnRect.left() < CnetTableColumn::EDGE_WIDTH;
      bool isOnRight =
        columnRect.right() - mousePos.x() < CnetTableColumn::EDGE_WIDTH;
      bool isResizable = false;

      CnetTableColumnList visCols = columns->getVisibleColumns();
      if (isOnLeft && columnNum > 0)
        isResizable = visCols[columnNum - 1]->getTitle().size();
      else
        if (isOnRight)
          isResizable = visCols[columnNum]->getTitle().size();

      isAtColumnEdge = (isOnLeft || isOnRight) && isResizable;
    }

    return isAtColumnEdge;
  }

  void CnetTableViewHeader::paintHeader(QPainter * painter, int rowHeight)
  {
    int visibleColWidth = -horizontalOffset;
    foreach(CnetTableColumn * col, columns->getVisibleColumns())
    {
      visibleColWidth += col->getWidth() - 1;
    }

    QRect rect(0, 0, qMin(width(), visibleColWidth), rowHeight);

    int x = rect.center().x();
    QLinearGradient gradient(x, rect.top(), x, rect.bottom());

    //FIXME: selected needs to be member variable
    bool selected = false;
    QColor color = selected ? palette().highlight().color() :
        palette().button().color();

    // create gradient and fill header area with it
    int adjustment = 110;
    gradient.setColorAt(0, color.lighter(adjustment));
    gradient.setColorAt(1, color.darker(adjustment));
    painter->fillRect(rect, gradient);

    // Save off composition mode and brush, which will need to be restored
    // after the progress is painted.
    QBrush brush = painter->brush();
    QPainter::CompositionMode compMode = painter->compositionMode();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    // draw filter progress (if not at 100%)
    painter->setBrush(QBrush(QColor(0, 70, 100, 30)));
    paintProgress(painter, rect, filterProgressMin, filterProgressMax,
        filterProgress);

    // draw rebuild progress (if not at 100%)
    painter->setBrush(QBrush(QColor(100, 70, 0, 30)));
    paintProgress(painter, rect, rebuildProgressMin, rebuildProgressMax,
        rebuildProgress);

    // draw the header's text. A rect will be drawn for each of the columns,
    // with the column text in the center of it.
    painter->setBrush(brush);
    painter->setCompositionMode(compMode);

    foreach(CnetTableColumn * visibleCol, columns->getVisibleColumns())
    {
      QString columnText = visibleCol->getTitle();
      QRect columnRect(
        getColumnRect(columns->getVisibleColumns().indexOf(visibleCol)));
      QPen pen(palette().dark().color().darker(150));
      pen.setCapStyle(Qt::RoundCap);
      painter->setPen(pen);
      painter->drawLine(columnRect.topLeft() + QPoint(0, 1),
          columnRect.bottomLeft() + QPoint(0, 1));
      painter->drawLine(columnRect.topLeft() + QPoint(1, 0),
          columnRect.topRight() - QPoint(0, 0));
      painter->drawLine(columnRect.topLeft() + QPoint(1, 1),
          columnRect.topRight() + QPoint(0, 1));
      painter->drawLine(columnRect.bottomLeft() + QPoint(1, 1),
          columnRect.bottomRight() + QPoint(0, 1));
      painter->drawLine(columnRect.bottomLeft() + QPoint(1, 1),
          columnRect.bottomRight() + QPoint(0, 1));
      painter->drawLine(columnRect.topRight() + QPoint(0, 1),
          columnRect.bottomRight() - QPoint(0, 0));
      painter->setPen(selected ? palette().highlightedText().color() :
          palette().buttonText().color());
      painter->drawText(columnRect, Qt::AlignCenter | Qt::TextSingleLine,
          columnText);

      // Move the column rect to the position of the next column.
      columnRect.moveLeft(columnRect.right());
    }
  }


  void CnetTableViewHeader::paintProgress(QPainter * painter,
      const QRect & rect, int min, int max, int value)
  {
    // draw filter progress if not at 100%
    double progressPercent = 1.0;
    int progressRange = max - min;
    if (progressRange > 0)
      progressPercent = ((double)(value - min)) / progressRange;

    if (progressPercent < 1.0)
    {
      QRect progressRect(rect);
      progressRect.setWidth((int)(progressRect.width() * progressPercent));
      painter->fillRect(progressRect, painter->brush());
    }
  }


  void CnetTableViewHeader::updateFilterProgress(int newProgress)
  {
    filterProgress = newProgress;
    update();
  }


  void CnetTableViewHeader::updateFilterProgressRange(int min, int max)
  {
    filterProgressMin = min;
    filterProgressMax = max;
    update();
  }


  void CnetTableViewHeader::updateRebuildProgress(int newProgress)
  {
    rebuildProgress = newProgress;
    update();
  }


  void CnetTableViewHeader::updateRebuildProgressRange(int min, int max)
  {
    rebuildProgressMin = min;
    rebuildProgressMax = max;
    update();
  }
}

