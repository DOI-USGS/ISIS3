#include "IsisDebug.h"

#include "TableViewHeader.h"

#include <iostream>

#include <QAbstractItemModel>
#include <QFontMetrics>
#include <QLabel>
#include <QLinearGradient>
#include <QLocale>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QString>
#include <QVBoxLayout>

#include "TableColumn.h"
#include "TableColumnList.h"
#include "TableView.h"
#include "AbstractTreeModel.h"
#include "AbstractTableModel.h"


namespace Isis {
  namespace CnetViz {
    TableViewHeader::TableViewHeader(AbstractTableModel *someModel) {
      nullify();

      m_horizontalOffset = 0;
      m_filterProgress = 0;
      m_filterProgressMin = 0;
      m_filterProgressMax = 0;
      m_rebuildProgress = 0;
      m_rebuildProgressMin = 0;
      m_rebuildProgressMax = 0;
      m_sortProgress = 0;
      m_sortProgressMin = 0;
      m_sortProgressMax = 0;
      m_visibleCount = -1;
      m_totalCount = -1;

      m_clickedColumnEdge = -1;
      m_clickedColumn = -1;

      setMouseTracking(true);

      setModel(someModel);

      ARROW_HEIGHT = 3;
      ARROW_WIDTH = 5;
    }


    TableViewHeader::~TableViewHeader() {
      m_columns = NULL;
    }


    void TableViewHeader::setColumns(TableColumnList *cols) {
      m_columns = cols;
    }


    QSize TableViewHeader::minimumSizeHint() const {
      return QSize(0, QFontMetrics(font()).height() + 6);
      /*QFontMetrics(font()).width(text->join("")) + 15,
          QFontMetrics(font()).height() + 6);*/
    }


    QSize TableViewHeader::sizeHint() const {
      return minimumSizeHint();
    }


    void TableViewHeader::setModel(AbstractTableModel *someModel) {
      if (m_model) {
        disconnect(m_model, SIGNAL(filterProgressChanged(int)),
            this, SLOT(updateFilterProgress(int)));
        disconnect(m_model, SIGNAL(rebuildProgressChanged(int)),
            this, SLOT(updateRebuildProgress(int)));
        disconnect(m_model, SIGNAL(sortProgressChanged(int)),
            this, SLOT(updateSortProgress(int)));
        disconnect(m_model, SIGNAL(filterProgressRangeChanged(int, int)),
            this, SLOT(updateFilterProgressRange(int, int)));
        disconnect(m_model, SIGNAL(rebuildProgressRangeChanged(int, int)),
            this, SLOT(updateRebuildProgressRange(int, int)));
        disconnect(m_model, SIGNAL(sortProgressRangeChanged(int, int)),
            this, SLOT(updateSortProgressRange(int, int)));
        disconnect(m_model, SIGNAL(filterCountsChanged(int, int)),
            this, SLOT(handleFilterCountsChanged(int, int)));
        disconnect(this, SIGNAL(requestedGlobalSelection(bool)),
            m_model, SLOT(setGlobalSelection(bool)));
        disconnect(m_model, SIGNAL(m_modelModified()),
            this, SLOT(update()));
      }

      m_model = someModel;

      connect(m_model, SIGNAL(filterProgressChanged(int)),
          this, SLOT(updateFilterProgress(int)));
      connect(m_model, SIGNAL(rebuildProgressChanged(int)),
          this, SLOT(updateRebuildProgress(int)));
      connect(m_model, SIGNAL(sortProgressChanged(int)),
          this, SLOT(updateSortProgress(int)));
      connect(m_model, SIGNAL(filterProgressRangeChanged(int, int)),
          this, SLOT(updateFilterProgressRange(int, int)));
      connect(m_model, SIGNAL(rebuildProgressRangeChanged(int, int)),
          this, SLOT(updateRebuildProgressRange(int, int)));
      connect(m_model, SIGNAL(sortProgressRangeChanged(int, int)),
          this, SLOT(updateSortProgressRange(int, int)));
      connect(m_model, SIGNAL(filterCountsChanged(int, int)),
          this, SLOT(handleFilterCountsChanged(int, int)));
      connect(this, SIGNAL(requestedGlobalSelection(bool)),
          m_model, SLOT(setGlobalSelection(bool)));
      connect(m_model, SIGNAL(modelModified()), this, SLOT(update()));


      if (m_columns) {
        for (int i = 0; i < m_columns->size(); i++) {
          disconnect((*m_columns)[i], SIGNAL(visibilityChanged()),
              this, SLOT(update()));
        }
      }

      m_columns = m_model->getColumns();

      for (int i = 0; i < m_columns->size(); i++)
        connect((*m_columns)[i], SIGNAL(visibilityChanged()), this, SLOT(update()));
    }


    void TableViewHeader::handleFilterCountsChanged(
      int visibleTopLevelItemCount, int topLevelItemCount) {
      m_visibleCount = visibleTopLevelItemCount;
      m_totalCount = topLevelItemCount;

      if (m_visibleCount >= 0) {
        TableColumnList visibleCols = m_columns->getVisibleColumns();
        for (int i = 0; i < visibleCols.size(); i++) {
          TableColumn *& col = visibleCols[i];

          if (col->getTitle().isEmpty())
            col->setWidth(QFontMetrics(font()).width(
                QString::number(m_visibleCount)) + 22);
        }
      }

      updateGeometry();
      update();
    }


    void TableViewHeader::updateHeaderOffset(int newOffset) {
      m_horizontalOffset = newOffset;
      update();
    }


    void TableViewHeader::mousePressEvent(QMouseEvent *event) {
      QPoint mousePos = event->pos();

      m_clickedColumn = getMousedColumn(mousePos);

      //     QRect priorityRect = getSortingPriorityRect(m_clickedColumn);
      //     QRect arrowRect = getSortingArrowRect(m_clickedColumn);


      if (event->buttons() == Qt::LeftButton) {
        m_clickedColumnEdge = getMousedColumnEdge(mousePos);
        if (m_clickedColumnEdge == -1 && m_clickedColumn != -1) {
          // The click wasn't on a column edge.
          if (m_columns->getVisibleColumns()[m_clickedColumn]->getTitle().isEmpty()) {
            emit requestedGlobalSelection(true);
          }
          else {
            //           if (priorityRect.contains(mousePos))
            //           {
            //             emit requestedColumnSelection(m_clickedColumn, true);
            //           }

          }
        }
      }
    }


    void TableViewHeader::mouseMoveEvent(QMouseEvent *event) {
      QPoint mousePos = event->pos();

      if (m_clickedColumnEdge >= 0) {
        // edge == column that we want to resize
        QRect columnToResizeRect(getColumnRect(m_clickedColumnEdge));
        columnToResizeRect.setRight(mousePos.x());

        TableColumn *col = m_columns->getVisibleColumns()[m_clickedColumnEdge];

        int newWidth = 1;
        if (columnToResizeRect.width() > 1) {
          newWidth = columnToResizeRect.width();
          if (m_columns->getSortingOrder()[0] == col)
            newWidth = qMax(newWidth, ARROW_WIDTH + SORT_ARROW_MARGIN * 2);
        }

        m_columns->getVisibleColumns()[m_clickedColumnEdge]->setWidth(newWidth);
      }

      if (mouseAtResizableColumnEdge(mousePos)) {
        setCursor(Qt::SizeHorCursor);
      }
      else {
        setCursor(Qt::ArrowCursor);
      }

      update();
    }


    void TableViewHeader::mouseReleaseEvent(QMouseEvent *event) {
      bool wasLastCol =
        m_clickedColumnEdge >= m_columns->getVisibleColumns().size() - 2;
      if (m_clickedColumnEdge != -1) {
        emit columnResized(wasLastCol);
      }
      else {
        if (m_clickedColumn == getMousedColumn(event->pos())) {
          TableColumn *col = m_columns->getVisibleColumns()[m_clickedColumn];

          TableColumn const *sortCol =
            m_columns->getVisibleColumns().getSortingOrder()[0];

          if (col == sortCol)
            col->setSortAscending(!col->sortAscending());
          else
            m_columns->raiseToTop(col);

          if (!m_model->sortingOn()) {
            QMessageBox::information(this, tr("Sorting Disabled"),
                tr("Sorting is currently disabled for this table. Please configure your sorting "
                    "options before trying to sort by [<font color='red'>%1</font>].")
                .arg(col->getTitle()),
                QMessageBox::Ok);
          }
        }
      }

      m_clickedColumnEdge = -1;
      m_clickedColumn = -1;

      update();
    }


    void TableViewHeader::paintEvent(QPaintEvent *event) {
      QPainter painter(this);
      painter.setRenderHints(QPainter::Antialiasing |
          QPainter::TextAntialiasing);

      ARROW_HEIGHT = qMax(height() / 5, 3);
      ASSERT(height() > 8);
      ARROW_WIDTH = ARROW_HEIGHT * 2 - 1;

      paintHeader(&painter, height());
      //    painter.drawRect(0, 0, width(), height());
      painter.end();
    }


    void TableViewHeader::nullify() {
      m_model = NULL;
      m_columns = NULL;
    }


    QRect TableViewHeader::getColumnRect(int column) const {
      QRect colRect;

      TableColumnList visibleCols = m_columns->getVisibleColumns();

      if (column < visibleCols.size() && column >= 0) {
        int indent = 1;

        for (int i = 0; i < column; i++)
          indent += visibleCols[i]->getWidth() - 1;

        colRect = QRect(indent - m_horizontalOffset, 0,
            visibleCols[column]->getWidth(), height());
      }

      return colRect;
    }


    int  TableViewHeader::getMousedColumn(QPoint mousePos) {
      int mousedColumn = -1;

      for (int i = 0;
          i < m_columns->getVisibleColumns().size() && mousedColumn < 0;
          i++) {
        QRect columnRect(getColumnRect(i));
        if (columnRect.contains(mousePos))
          mousedColumn = i;
      }

      return mousedColumn;
    }


    int TableViewHeader::getMousedColumnEdge(QPoint mousePos) {
      int edge = -1;

      if (mouseAtResizableColumnEdge(mousePos)) {
        int mousedColumn = getMousedColumn(mousePos);

        QRect columnRect(getColumnRect(mousedColumn));

        // mouseAtResizableColumnEdge can't be on left of first so this won't
        // return -1 resulting from this particular logic.
        if (mousePos.x() - columnRect.left() < TableColumn::EDGE_WIDTH)
          edge = mousedColumn - 1;
        else
          edge = mousedColumn;
      }

      return edge;
    }


    bool TableViewHeader::mouseAtResizableColumnEdge(QPoint mousePos) {
      int columnNum = getMousedColumn(mousePos);

      QRect columnRect(getColumnRect(columnNum));

      bool isAtColumnEdge = false;

      if (!columnRect.isNull()) {
        bool isOnLeft =
          mousePos.x() - columnRect.left() < TableColumn::EDGE_WIDTH;
        bool isOnRight =
          columnRect.right() - mousePos.x() < TableColumn::EDGE_WIDTH;
        bool isResizable = false;

        TableColumnList visCols = m_columns->getVisibleColumns();
        if (isOnLeft && columnNum > 0)
          isResizable = visCols[columnNum - 1]->getTitle().size();
        else if (isOnRight)
          isResizable = visCols[columnNum]->getTitle().size();

        isAtColumnEdge = (isOnLeft || isOnRight) && isResizable;
      }

      return isAtColumnEdge;
    }


    void TableViewHeader::paintHeader(QPainter *painter, int rowHeight) {
      int visibleColWidth = -m_horizontalOffset;
      TableColumnList visibleCols = m_columns->getVisibleColumns();

      for (int i = 0; i < visibleCols.size(); i++)
        visibleColWidth += visibleCols[i]->getWidth() - 1;

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
      paintProgress(painter, rect, m_filterProgressMin, m_filterProgressMax,
          m_filterProgress, false);

      // draw rebuild progress (if not at 100%)
      painter->setBrush(QBrush(QColor(100, 70, 0, 30)));
      paintProgress(painter, rect, m_rebuildProgressMin, m_rebuildProgressMax,
          m_rebuildProgress, false);

      // draw sort progress (if not at 100%)
      painter->setBrush(QBrush(QColor(0, 100, 0, 30)));
      paintProgress(painter, rect, m_sortProgressMin, m_sortProgressMax,
          m_sortProgress, true);

      // draw the header's text. A rect will be drawn for each of the m_columns,
      // with the column text in the center of it.
      painter->setBrush(brush);
      painter->setCompositionMode(compMode);

      for (int i = 0; i < visibleCols.size(); i++) {
        TableColumn *visibleCol = visibleCols[i];

        QString columnText = visibleCol->getTitle();
        QRect columnRect(getColumnRect(visibleCols.indexOf(visibleCol)));
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

        QRect textRect(columnRect.x(),
            columnRect.y(),
            columnRect.width() - (SORT_ARROW_MARGIN * 2 + ARROW_WIDTH),
            columnRect.height());
        painter->drawText(textRect , Qt::AlignCenter | Qt::TextSingleLine,
            columnText);

        if (visibleCol == visibleCols.getSortingOrder()[0] &&
            visibleCol->getWidth() >= SORT_ARROW_MARGIN * 2 + ARROW_WIDTH) {
          ASSERT(SORT_ARROW_MARGIN > 0);

          QRect arrowRect(textRect.right() + 1,
              textRect.y(),
              SORT_ARROW_MARGIN * 2 + ARROW_WIDTH,
              textRect.height());

          ASSERT(arrowRect.width() + textRect.width() == columnRect.width());
          ASSERT(arrowRect.right() == columnRect.right());


          // assume ascending order (arrow looks like v)
          QPoint left(arrowRect.left() + SORT_ARROW_MARGIN,
              arrowRect.center().y() - ((ARROW_HEIGHT - 1) / 2));

          int yOffSet = ((ARROW_HEIGHT - 1) / 2);
          if (ARROW_HEIGHT % 2 == 0)
            yOffSet++;
          QPoint center(left.x() + ((ARROW_WIDTH - 1) / 2),
              arrowRect.center().y() + yOffSet);

          QPoint right(center.x() + ((ARROW_WIDTH - 1) / 2),
              arrowRect.center().y() - ((ARROW_HEIGHT - 1) / 2));

          ASSERT(right.x() == arrowRect.right() - SORT_ARROW_MARGIN);
          ASSERT(right.x() - center.x() == center.x() - left.x());

          if (!visibleCol->sortAscending()) {
            // flip arrow (to look like ^)
            left.setY(center.y());
            center.setY(right.y());
            right.setY(left.y());
          }

          if (m_model->sortingOn()) {
            painter->drawLine(left, center);
            painter->drawLine(center, right);
          }
        }

        // Move the column rect to the position of the next column.
        columnRect.moveLeft(columnRect.right());
      }
    }


    void TableViewHeader::paintProgress(QPainter *painter,
        const QRect &rect, int min, int max, int value, bool over100) {
      double progressPercent = 1.0;
      int progressRange = max - min;

      if (progressRange > 0)
        progressPercent = ((double)(value - min)) / progressRange;
      else if (progressRange == 0)
        progressPercent = 0;

      if (progressPercent < 1.0 || over100) {
        QRect progressRect(rect);
        progressRect.setWidth((int)(progressRect.width() * progressPercent));
        painter->fillRect(progressRect, painter->brush());
      }
    }


    void TableViewHeader::updateFilterProgress(int newProgress) {
      m_filterProgress = newProgress;
      update();
    }


    void TableViewHeader::updateFilterProgressRange(int min, int max) {
      m_filterProgressMin = min;
      m_filterProgressMax = max;
      update();
    }


    void TableViewHeader::updateRebuildProgress(int newProgress) {
      m_rebuildProgress = newProgress;
      update();
    }


    void TableViewHeader::updateRebuildProgressRange(int min, int max) {
      m_rebuildProgressMin = min;
      m_rebuildProgressMax = max;
      update();
    }


    void TableViewHeader::updateSortProgress(int newProgress) {
      m_sortProgress = newProgress;
      update();
    }


    void TableViewHeader::updateSortProgressRange(int min, int max) {
      m_sortProgressMin = min;
      m_sortProgressMax = max;
      update();
    }
  }
}
