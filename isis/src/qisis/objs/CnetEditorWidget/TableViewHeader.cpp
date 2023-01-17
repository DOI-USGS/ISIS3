/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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

#include "AbstractTableModel.h"
#include "AbstractTreeModel.h"
#include "TableColumn.h"
#include "TableColumnList.h"
#include "TableView.h"


namespace Isis {
  /**
    * Constructor
    *
    * @param someModel The table model
    */
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


  /**
    * Destructor
    */
  TableViewHeader::~TableViewHeader() {
    m_columns = NULL;
  }


  /**
    * Sets the column list
    *
    * @param cols The column list
    */
  void TableViewHeader::setColumns(TableColumnList *cols) {
    m_columns = cols;
  }


  /**
    * Returns the minimum size based on the font
    *
    * @return QSize Minimum size allowed
    */
  QSize TableViewHeader::minimumSizeHint() const {
    return QSize(0, QFontMetrics(font()).height() + 6);
    /*QFontMetrics(font()).width(text->join("")) + 15,
        QFontMetrics(font()).height() + 6);*/
  }


  /**
    * Returns the minimum size based on the font
    *
    * @see minimumSizeHint
    *
    * @return QSize Minimum size allowed
    */
  QSize TableViewHeader::sizeHint() const {
    return minimumSizeHint();
  }


  /**
    * Connects the table model to the functions that handle changes
    *
    * @param someModel The table model to connect
    */
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


  /**
    * Updates the visible columns, and geometry when the filter count changes
    *
    * @param visibleTopLevelItemCount Number of visible top level items
    * @param topLevelItemCount Number of top level items
    */
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


  /**
    * Updates the header offset
    *
    * @param newOffset The new header offset
    */
  void TableViewHeader::updateHeaderOffset(int newOffset) {
    m_horizontalOffset = newOffset;
    update();
  }


  /**
    * Overrides QWidget::mousePressEvent
    *
    * @param event The mouse press event
    */
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


  /**
    * Overrides QWidget::mouseMoveEvent
    *
    * @param event The mouse move event
    */
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


  /**
    * Overrides QWidget::mouseReleaseEvent
    *
    * @param event The mouse release event
    */
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


  /**
    * Repaints the header
    *
    * @param event The paint event
    */
  void TableViewHeader::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing);

    ARROW_HEIGHT = qMax(height() / 5, 3);

    ARROW_WIDTH = ARROW_HEIGHT * 2 - 1;

    paintHeader(&painter, height());
    //    painter.drawRect(0, 0, width(), height());
    painter.end();
  }


  /**
    * Sets all the member variables to NULL
    */
  void TableViewHeader::nullify() {
    m_model = NULL;
    m_columns = NULL;
  }


  /**
    * Returns the visible column rectangle
    *
    * @param column Column to check
    *
    * @return QRect The visible column rectangle
    */
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


  /**
    * Returns the column under the mouse
    *
    * @param mousePos The mouse position
    *
    * @return int Index of the column under the mouse
    */
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


  /**
    * Returns the edge of the column under the mouse
    *
    * @param mousePos The mouse position
    *
    * @return int Index of the column edge under the mouse
    */
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


  /**
    * Returns if the mouse is at the edge of a resizeable column
    *
    * @param mousePos The mouse position
    *
    * @return bool True if the mouse is at the edge fo a resizable column
    */
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


  /**
    * Repaints the header
    *
    * @param painter The QPainter
    * @param rowHeight The new row height
    */
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

        QRect arrowRect(textRect.right() + 1,
            textRect.y(),
            SORT_ARROW_MARGIN * 2 + ARROW_WIDTH,
            textRect.height());

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


  /**
    * Updates the progress bar
    *
    * @param painter The QPainter
    * @param rect The progress bar
    * @param min The minimum progress value
    * @param max The maximum progress value
    * @param value Current progress value
    * @param over100 Bool if the progress can go over 100
    */
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


  /**
    * Updates the current filter progress value
    *
    * @param newProgress New progress value
    */
  void TableViewHeader::updateFilterProgress(int newProgress) {
    m_filterProgress = newProgress;
    update();
  }


  /**
    * Updates the range of the filter progress
    *
    * @param min The minimum progress
    * @param max The maximum progress
    */
  void TableViewHeader::updateFilterProgressRange(int min, int max) {
    m_filterProgressMin = min;
    m_filterProgressMax = max;
    update();
  }


  /**
    * Updates the current rebuild progress value
    *
    * @param newProgress New progress value
    */
  void TableViewHeader::updateRebuildProgress(int newProgress) {
    m_rebuildProgress = newProgress;
    update();
  }


  /**
    * Updates the range of the rebuild progress
    *
    * @param min The minimum progress
    * @param max The maximum progress
    */
  void TableViewHeader::updateRebuildProgressRange(int min, int max) {
    m_rebuildProgressMin = min;
    m_rebuildProgressMax = max;
    update();
  }


  /**
    * Updates the current sort progress value
    *
    * @param newProgress New progress value
    */
  void TableViewHeader::updateSortProgress(int newProgress) {
    m_sortProgress = newProgress;
    update();
  }


  /**
    * Updates the range of the sort progress
    *
    * @param min The minimum progress
    * @param max The maximum progress
    */
  void TableViewHeader::updateSortProgressRange(int min, int max) {
    m_sortProgressMin = min;
    m_sortProgressMax = max;
    update();
  }
}
