#include "IsisDebug.h"

#include "CnetTableViewContent.h"

#include <cmath>
#include <iostream>

#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMutex>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QScrollBar>
#include <QSize>
#include <QStyle>
#include <QStyleOptionViewItemV4>
#include <QVBoxLayout>

#include "iException.h"
#include "iString.h"

#include "AbstractCnetTableDelegate.h"
#include "AbstractCnetTableModel.h"
#include "AbstractTreeItem.h"
#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"


using std::cerr;


namespace Isis
{

  CnetTableViewContent::CnetTableViewContent(CnetTableColumnList * cols)
  {
    nullify();

//     parentView = (CnetTableView *) parent;

    items = new QList< AbstractTreeItem * >;
    mousePressPos = new QPoint;
    activeCell = new QPair<AbstractTreeItem *, int>(NULL, -1);
    rowsWithActiveColumnSelected = new QList<AbstractTreeItem *>;
    lastShiftSelection = new QList<AbstractTreeItem *>;

    verticalScrollBar()->setSingleStep(1);
//     horizontalScrollBar()->setSingleStep(10);

    rowHeight = QFontMetrics(font()).height() + ITEM_PADDING;
    ASSERT(rowHeight > 0);

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
        this, SIGNAL(horizontalScrollBarValueChanged(int)));

    setMouseTracking(true);
//     setContextMenuPolicy(Qt::ActionsContextMenu);
//     QAction * alternateRowsAct = new QAction("&Alternate row colors", this);
//     alternateRowsAct->setCheckable(true);
//     connect(alternateRowsAct, SIGNAL(toggled(bool)),
//         this, SLOT(setAlternatingRowColors(bool)));
//     addAction(alternateRowsAct);
//     alternateRowsAct->setChecked(true);
    if (!cols)
    {
      iString msg = "NULL column list passed to CnetTableViewContent "
          "constructor!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    columns = cols;
    updateHorizontalScrollBar();

    createActions();

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
  }


  CnetTableViewContent::~CnetTableViewContent()
  {
    delete items;
    items = NULL;

    delete mousePressPos;
    mousePressPos = NULL;

    delete activeCell;
    activeCell = NULL;

    delete editWidget;
    editWidget = NULL;

    delete lastShiftSelection;
    lastShiftSelection = NULL;

    delete applyToSelectionAct;
    applyToSelectionAct = NULL;

    delete applyToAllAct;
    applyToAllAct = NULL;

    delete deleteSelectedRowsAct;
    deleteSelectedRowsAct = NULL;

    columns = NULL;
  }


  QSize CnetTableViewContent::minimumSizeHint() const
  {
    return QWidget::minimumSizeHint();
  }


  QSize CnetTableViewContent::sizeHint()
  {
    return minimumSizeHint();
  }


  AbstractCnetTableModel * CnetTableViewContent::getModel()
  {
    return model;
  }


  void CnetTableViewContent::setModel(AbstractCnetTableModel * someModel)
  {
    if (!someModel)
    {
      iString msg = "Attempted to set a NULL model!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if (model)
    {
      disconnect(model, SIGNAL(modelModified()), this, SLOT(refresh()));
      disconnect(model, SIGNAL(filterProgressChanged(int)),
          this, SLOT(updateItemList()));
    }

    model = someModel;
    connect(model, SIGNAL(modelModified()), this, SLOT(refresh()));
    connect(model, SIGNAL(filterProgressChanged(int)),
        this, SLOT(updateItemList()));
    connect(this, SIGNAL(modelDataChanged()),
            model, SLOT(applyFilter()));

    refresh();
  }


  void CnetTableViewContent::refresh()
  {
//     cerr << "CnetTableViewContent::refresh called\n";
    if (model)
    {
      if (!model->isFiltering())
      {
        int rowCount = model->getVisibleRowCount();
        verticalScrollBar()->setRange(0, qMax(rowCount - 1, 0));
      }
      
      updateItemList();
      activeCell->first = NULL;
      activeCell->second = -1;
      clearColumnSelection();
      lastDirectlySelectedRow = NULL;
      lastShiftSelection->clear();

      if (model->getSelectedItems().size() &&
          rowsWithActiveColumnSelected->size())
      {
        lastDirectlySelectedRow = NULL;
        clearColumnSelection();
      }

      viewport()->update();
    }
//     cerr << "CnetTableViewContent::refresh done\n";
  }


  void CnetTableViewContent::updateHorizontalScrollBar(bool scrollRight)
  {
    if (columns)
    {
      int range = 0;
      foreach(CnetTableColumn * col, columns->getVisibleColumns())
      range += col->getWidth() - 1;
      // For the border...
      range -= 2;
      horizontalScrollBar()->setRange(0, range - viewport()->width());
      horizontalScrollBar()->setPageStep(viewport()->width());

      if (scrollRight)
        horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
    }
  }


  bool CnetTableViewContent::eventFilter(QObject * target, QEvent * event)
  {
    return QObject::eventFilter(target, event);
  }


  void CnetTableViewContent::mouseDoubleClickEvent(QMouseEvent * event)
  {
    if (event->buttons() & Qt::LeftButton)
    {
//       updateActiveCell(event->pos());
      int rowNum = event->pos().y() / rowHeight;

      if (activeCell->first && isCellEditable(rowNum, activeCell->second))
      {
        const AbstractCnetTableDelegate * delegate = model->getDelegate();
        QString colTitle =
          columns->getVisibleColumns().at(activeCell->second)->getTitle();

        delete editWidget;
        editWidget = NULL;
        editWidget = delegate->getWidget(colTitle);
        delegate->readData(editWidget, activeCell->first, colTitle);
        editWidget->setParent(this);
        editWidget->setFocus(Qt::OtherFocusReason);
      }

      viewport()->update();
    }
  }


  void CnetTableViewContent::mousePressEvent(QMouseEvent * event)
  {
    if (event->buttons() & Qt::LeftButton)
    {
      // FIXME refactor this file (lol)
      if (!(event->modifiers() & Qt::ShiftModifier))
        updateActiveCell(event->pos());

      int rowNum = event->pos().y() / rowHeight;
      int colNum = getColumnFromScreenX(event->pos().x());
      
      // HACK
      // BUG
      // TODO
      // FIXME
      if (colNum == 0) {
        activeCell->first = NULL;
        activeCell->second = -1;
      }
      
      if (rowNum >= 0 && rowNum < items->size() && activeCell->first)
      {
        // The user clicked on a valid item, handle selection of individual
        // cells (not rows).

        // Deselect all rows, as this will now be a cell selection.
        model->setGlobalSelection(false);

        if (isCellEditable(rowNum, activeCell->second))
        {
          if (event->modifiers() & Qt::ControlModifier)
          {
            if (rowsWithActiveColumnSelected->indexOf(activeCell->first) < 0)
              rowsWithActiveColumnSelected->append(activeCell->first);
            else
              rowsWithActiveColumnSelected->removeAll(activeCell->first);
            lastDirectlySelectedRow = activeCell->first;
            
            lastShiftSelection->clear();
          }
          else
          {
            if (event->modifiers() & Qt::ShiftModifier)
            {
              updateColumnGroupSelection((*items)[rowNum]);
            }
            else
            {
              // Normal click, no modifiers.
              rowsWithActiveColumnSelected->clear();
              rowsWithActiveColumnSelected->append(activeCell->first);
              lastDirectlySelectedRow = activeCell->first;
              lastShiftSelection->clear();
            }
          }
        }
      }
      else
      {
        // row selections
        if (rowNum >= 0 && rowNum < items->size())
        {
          // There is no active cell, maybe they clicked on the row number column.
          int columnNum = getColumnFromScreenX(event->pos().x());

          if (columnNum != -1)
          {
            CnetTableColumn * column = columns->getVisibleColumns().at(columnNum);
            if (column->getTitle().isEmpty())
            {
              clearColumnSelection();

              if (event->modifiers() & Qt::ControlModifier)
              {
                items->at(rowNum)->setSelected(!items->at(rowNum)->isSelected());
                lastDirectlySelectedRow = items->at(rowNum);
              }
              else
              {
                if (event->modifiers() & Qt::ShiftModifier)
                {
                  updateRowGroupSelection(rowNum);
                }
                else
                {
                  model->setGlobalSelection(false);
                  items->at(rowNum)->setSelected(true);
                  lastDirectlySelectedRow = items->at(rowNum);
                }
              }
            }
          }
        }
      }

      delete editWidget;
      editWidget = NULL;

      viewport()->update();
      emit selectionChanged();
    }
  }


  void CnetTableViewContent::mouseReleaseEvent(QMouseEvent * event)
  {
  }


  void CnetTableViewContent::mouseMoveEvent(QMouseEvent * event)
  {
    if (!editWidget)
    {
      if (event->buttons() & Qt::LeftButton)
      {
        int rowNum = event->pos().y() / rowHeight;
        
        // used to make sure that the mouse position is inside the content
        int yPos = event->pos().y();
        if (yPos >= 0 && rowNum < items->size() && activeCell->first)
        {
          // The user clicked on a valid item, handle selection of individual cells
          // (not rows).
          if (isCellEditable(rowNum, activeCell->second))
          {
            updateColumnGroupSelection((*items)[rowNum]);
          }
        }
        else
        {
          // row selections
          if (yPos >= 0 && rowNum < items->size())
          {
            // There is no active cell, maybe they clicked on the row number column.
            int columnNum = getColumnFromScreenX(event->pos().x());

            if (columnNum != -1)
            {
              clearColumnSelection();
              updateRowGroupSelection(rowNum);
            }
          }
        }

        QScrollBar * vertScroll = verticalScrollBar();

        if (yPos > viewport()->height() && vertScroll->value() < vertScroll->maximum())
        {
          // Scroll down to allow for more drag selections.
          vertScroll->setValue(vertScroll->value() + 1);
        }
        else
        {
          if (yPos < 0 && vertScroll->value() > vertScroll->minimum())
            vertScroll->setValue(vertScroll->value() - 1);
        }

        viewport()->update();
        emit selectionChanged();
      }
    }
  }


  void CnetTableViewContent::leaveEvent(QEvent * event)
  {
//     viewport()->update();
  }


  void CnetTableViewContent::keyPressEvent(QKeyEvent * event)
  {
    // Ctrl-A selects all rows.
    if (event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier)
    {
      clearActiveCell();
      clearColumnSelection();
      model->setGlobalSelection(true);
      viewport()->update();

      emit selectionChanged();
    }
    else if (event->key() == Qt::Key_Delete)
    {
      if (hasRowSelection())
        deleteSelectedRows();
    }
    else if (event->key() == Qt::Key_Down)
    {

    }
    else
    {
      QWidget::keyPressEvent(event);
    }
  }


  void CnetTableViewContent::paintEvent(QPaintEvent * event)
  {
    if (model && columns)
    {
//       int startRow = verticalScrollBar()->value();
      int rowCount = (int) ceil(viewport()->height() / (double) rowHeight);

      QPainter painter(viewport());
      painter.setRenderHint(QPainter::Antialiasing, false);
//      painter.setRenderHints(QPainter::NonCosmeticDefaultPen, true);

      bool editWidgetVisible = false;
      for (int i = 0; i < rowCount; i++)
      {
        // define the top left corner of the row and also how big the row is
        QPoint relativeTopLeft(0, i * rowHeight);
        QPoint scrollBarPos(horizontalScrollBar()->value(),
            verticalScrollBar()->value());
        QPoint absoluteTopLeft(relativeTopLeft + scrollBarPos);
        QSize rowSize(viewport()->width(), (int) rowHeight);

        // Fill in the background with the background color
        painter.fillRect(QRect(relativeTopLeft, rowSize), palette().base());

        if (i < items->size())
        {
          if (items->at(i)->isSelected())
          {
            QPoint selectionTopLeft(-absoluteTopLeft.x(), relativeTopLeft.y());
            QSize selectionSize(columns->getVisibleWidth(), (int) rowHeight);

            QRect selectionRect(selectionTopLeft, selectionSize);
            painter.fillRect(selectionRect, palette().highlight().color());
          }

          paintRow(&painter, i, absoluteTopLeft, relativeTopLeft);
        }
      }

      for (int i = 0; i < rowCount; i++)
      {
        if (i < items->size())
        {
          QPoint relativeTopLeft(0, i * rowHeight);
          if (editWidget && activeCell->first == items->at(i))
          {
            QPair<int, int> xRange(
                columns->getVisibleXRange(activeCell->second));

            editWidget->move(
              QPoint(xRange.first - horizontalScrollBar()->value() - 1,
                  relativeTopLeft.y() + 1));
            editWidget->resize(xRange.second - xRange.first, rowHeight + 1);
            editWidget->setVisible(true);
            editWidgetVisible = true;
          }
          else
            if (activeCell->first == items->at(i))
            {
              QPair<int, int> activeXArea =
                columns->getVisibleXRange(activeCell->second);

              QRect activeArea(activeXArea.first, relativeTopLeft.y(),
                  activeXArea.second - activeXArea.first, rowHeight);

              activeArea.moveLeft(
                activeArea.left() - horizontalScrollBar()->value());
//          activeArea.setRight(activeArea.right() - 1);
//          activeArea.setBottom(activeArea.bottom() - 1);
              activeArea.adjust(-1, -1, -2, -1);
              QPen pen(Qt::black);
              pen.setWidth(3);
              painter.setPen(pen);
              painter.drawRect(activeArea);
            }
        }
      }

      if (editWidget && !editWidgetVisible)
        editWidget->setVisible(false);
    }
    else
    {
      QWidget::paintEvent(event);
    }
  }


  void CnetTableViewContent::resizeEvent(QResizeEvent * event)
  {
    QAbstractScrollArea::resizeEvent(event);
    updateHorizontalScrollBar();
    updateItemList();
//     refresh();
  }


  void CnetTableViewContent::scrollContentsBy(int dx, int dy)
  {
    QAbstractScrollArea::scrollContentsBy(dx, dy);
//     refresh();
   updateItemList();
  }


  void CnetTableViewContent::nullify()
  {
    parentView = NULL;
    model = NULL;
    items = NULL;
    activeCell = NULL;
    editWidget = NULL;
    lastDirectlySelectedRow = NULL;
    lastShiftSelection = NULL;
    mousePressPos = NULL;
    columns = NULL;
    rowsWithActiveColumnSelected = NULL;
    applyToSelectionAct = NULL;
    applyToAllAct = NULL;
    deleteSelectedRowsAct = NULL;
  }


  void CnetTableViewContent::cellDataChanged(const CnetTableColumn * col)
  {
    if (col->hasNetworkStructureEffect())
      emit rebuildModels(QList<AbstractTreeItem *>());
    emit modelDataChanged();
  }


  void CnetTableViewContent::clearActiveCell()
  {
    activeCell->first = NULL;
    activeCell->second = -1;
  }


  void CnetTableViewContent::clearColumnSelection()
  {
    rowsWithActiveColumnSelected->clear();
  }


  void CnetTableViewContent::copyCellSelection(bool allCells) {
    
    if (hasActiveCell()) {
      const CnetTableColumn * col = columns->getVisibleColumns().at(
          activeCell->second);

      QString colTitle = col->getTitle();

      // Is impossible unless bug exists.
      ASSERT(colTitle.size());

      // Grab the active cell's data and copy it to the selected cells that are
      // in the same column as the active cell.
      QString cellData = activeCell->first->getData(colTitle);

      if (allCells) {
        QList< AbstractTreeItem * > items = model->getItems(0, model->getVisibleRowCount());
        
        foreach (AbstractTreeItem * row, items) {
          row->setData(colTitle, cellData);
        }
      }
      else {
        foreach (AbstractTreeItem * row, *rowsWithActiveColumnSelected) {
          row->setData(colTitle, cellData);
        }
      }

      viewport()->update();
      cellDataChanged(col);
    }
  }


  void CnetTableViewContent::createActions() {
    applyToSelectionAct = new QAction(tr("Copy to selected cells"), this);
    applyToSelectionAct->setStatusTip(tr("Copy the contents of this cell to the"
                                         "selected cells"));
    connect(applyToSelectionAct, SIGNAL(triggered()),
            this, SLOT(copySelection()));

    applyToAllAct = new QAction(tr("Copy to all cells"), this);
    applyToAllAct->setStatusTip(tr("Copy the contents of this cell to all"
                                   "cells in the current column"));
    connect(applyToAllAct, SIGNAL(triggered()),
            this, SLOT(copyAll()));

    deleteSelectedRowsAct = new QAction(tr("Delete selected rows"), this);
    deleteSelectedRowsAct->setStatusTip(
        tr("Delete the currently selected rows"));
    connect(deleteSelectedRowsAct, SIGNAL(triggered()),
            this, SLOT(deleteSelectedRows()));
  }


  int CnetTableViewContent::getColumnFromScreenX(int screenX) const
  {
    int column = -1;

    for (int i = 0; column == -1 &&
        i < columns->getVisibleColumns().size(); i++)
    {
      QPair<int, int> cellXRange(columns->getVisibleXRange(i));
      int deltaX = -horizontalScrollBar()->value();

      if (cellXRange.first + deltaX < screenX &&
          cellXRange.second  + deltaX > screenX)
      {
        column = i;
      }
    }

    return column;
  }
  
  
  int CnetTableViewContent::getRowFromScreenY(int screenY) const
  {
    int rowNum = -1;
    int calculatedRowNum = screenY / rowHeight;
    
    if (calculatedRowNum >= 0 && calculatedRowNum < items->size() && 
        screenY >= 0 && screenY <= viewport()->height())
      rowNum = calculatedRowNum;
    
    return rowNum;
  }


  bool CnetTableViewContent::hasActiveCell() const {
    return (activeCell->first && activeCell->second >= 0);
  }


  bool CnetTableViewContent::hasRowSelection() const {
    return (model->getSelectedItems().size());
  }


  bool CnetTableViewContent::mouseInCellSelection(QPoint mousePos) const {
    int colNum = getColumnFromScreenX(mousePos.x());
    AbstractTreeItem * row = items->at(getRowFromScreenY(mousePos.y()));

    return (rowsWithActiveColumnSelected->contains(row) &&
            activeCell->second == colNum);
  }


  bool CnetTableViewContent::isMouseInRowSelection(QPoint mousePos) const {
    AbstractTreeItem * row = items->at(getRowFromScreenY(mousePos.y()));

    return (model->getSelectedItems().contains(row));
  }
  
  
  bool CnetTableViewContent::isRowValid(int rowNum) const {
    bool valid = false;
    
    if (rowNum >= 0 && rowNum < items->size())
      valid = true;
    
    return valid;
  }
  
  
  bool CnetTableViewContent::isColumnValid(int colNum) const {
    bool valid = false;
    
    if (colNum >= 0 && colNum < columns->getVisibleColumns().size())
      valid = true;
    
    return valid;
  }


  bool CnetTableViewContent::isCellEditable(int rowNum, int colNum) const
  {
    ASSERT(rowNum >= 0 && rowNum < items->size());
    ASSERT(colNum >= 0 && colNum < columns->getVisibleColumns().size());

    bool editable = false;

    if (items->at(rowNum)->isSelectable() &&
        !columns->getVisibleColumns().at(colNum)->isReadOnly())
      editable = true;

    return editable;
  }


  bool CnetTableViewContent::isDataColumn(int colNum) const {
    return columns->getVisibleColumns().at(colNum)->getTitle().size();
  }


  void CnetTableViewContent::paintRow(QPainter * painter, int rowNum,
      QPoint absolutePosition, QPoint relativePosition)
  {
    ASSERT(items);
    ASSERT(rowNum >= 0 && rowNum < items->size());

    QPoint point(-absolutePosition.x(), relativePosition.y());

    AbstractTreeItem * item = (*items)[rowNum];

    // should always be true, but prevents segfault in case of bug
    if (item)
    {
      QPen originalPen = painter->pen();

      QPoint textPoint(point.x() + ITEM_INDENTATION,
          point.y() + ITEM_PADDING / 2);

      // finally draw the text
      int textHeight = rowHeight - ITEM_PADDING;

      QFontMetrics metrics(font());

      QPen gridPen(Qt::gray);

      ASSERT(columns);
      CnetTableColumnList visibleCols = columns->getVisibleColumns();
      for (int i = 0; i < visibleCols.size(); i++)
      {
        // draw text
        QPair<int, int> cellXRange(visibleCols.getVisibleXRange(i));
        QRect cellRect(cellXRange.first, point.y(),
            cellXRange.second - cellXRange.first, rowHeight);
        cellRect.moveLeft(cellRect.left() - horizontalScrollBar()->value() - 1);

        QString columnTitle = visibleCols.at(i)->getTitle();
        QRect textRect(textPoint, QSize(cellRect.right() - textPoint.x(),
            textHeight));
        QString text;
        bool textCentered = false;
        if (!columnTitle.isEmpty())
        {
          text = item->getData(columnTitle);

          if (rowsWithActiveColumnSelected->indexOf(item) != -1 &&
              activeCell->second == i)
          {
            // This cell is selected, so render it as such.
            if (activeCell->first != item)
            {
              painter->fillRect(cellRect, QBrush(palette().highlight().color()));
              painter->setPen(palette().highlightedText().color());
            }
            else
            {
              painter->setPen(palette().text().color());
            }
          }
          else
          {
            if (item->isSelected())
            {
              painter->setPen(palette().highlightedText().color());
            }
            else
            {
              if (!isCellEditable(rowNum, i))
              {
                // The cell is not editable, so make the text grayed out.
                painter->setPen(palette().color(QPalette::Disabled,
                    QPalette::Text));
              }
              else
              {
                painter->setPen(palette().text().color());
              }
            }
          }
        }
        else
        {
          text = QString::number(rowNum + verticalScrollBar()->value());
          textCentered = true;

          // We need to paint the row number column.
          int x = cellRect.center().x();
          QLinearGradient gradient(x, cellRect.top(), x, cellRect.bottom());

          bool selected = item->isSelected();
          QColor color = selected ? palette().highlight().color() :
              palette().button().color();

          int adjustment = 110;
          gradient.setColorAt(0, color.lighter(adjustment));
          gradient.setColorAt(1, color.darker(adjustment));
          painter->fillRect(cellRect, gradient);
          if (selected)
            painter->setPen(palette().highlightedText().color());
          else
            painter->setPen(palette().text().color());
        }

        int flags = Qt::TextDontClip;
        if (textCentered)
          flags |= Qt::AlignCenter;

        painter->drawText(textRect, flags,
            metrics.elidedText(text, Qt::ElideRight,
                textRect.width() - ITEM_INDENTATION));

        textPoint.setX(cellRect.right() + ITEM_INDENTATION);
        painter->setPen(originalPen);

        painter->setPen(gridPen);
        painter->drawLine(QPoint(cellRect.right(), point.y()),
            QPoint(cellRect.right(), point.y() + rowHeight));
        painter->setPen(originalPen);
      }

      int left = -horizontalScrollBar()->value() - 1;
      int right = columns->getVisibleWidth();

      gridPen.setWidth(2);
      painter->setPen(gridPen);
      painter->drawLine(QPoint(left, point.y() + rowHeight),
          QPoint(right, point.y() + rowHeight));
      painter->setPen(originalPen);
    }
  }


  void CnetTableViewContent::updateActiveCell(QPoint screenPos)
  {
    if (editWidget && activeCell->first && activeCell->second >= 0)
    {
      try
      {
        const CnetTableColumn * col =
            columns->getVisibleColumns().at(activeCell->second);
        model->getDelegate()->saveData(editWidget, activeCell->first,
            col->getTitle());
        
        cellDataChanged(col);
      }
      catch (iException & e)
      {
        QMessageBox::critical(this, "Failed to Set Data", e.what());
        e.Clear();
      }
    }

    int rowNum = getRowFromScreenY(screenPos.y());
//     int colNum = getColumnFromScreenX(screenPos.x());
    int oldActiveColumn = activeCell->second;

    activeCell->first = NULL;
    activeCell->second = -1;

    if (rowNum >= 0)
    {
      AbstractTreeItem * item = (*items)[rowNum];

      CnetTableColumnList visibleCols = columns->getVisibleColumns();
      for (int i = 0; i < visibleCols.size(); i++)
      {
        QPair<int, int> cellXRange(columns->getVisibleXRange(i));
        QRect cellRect(cellXRange.first, rowHeight * rowNum,
            cellXRange.second - cellXRange.first, rowHeight);
        
        cellRect.moveLeft(cellRect.left() - horizontalScrollBar()->value());

        if (cellRect.contains(screenPos))
        {
          if (oldActiveColumn != -1 || !visibleCols.at(i)->getTitle().isEmpty())
          {
            activeCell->first = item;
            activeCell->second = i;
          }
        }
      }
    }

    if (oldActiveColumn != activeCell->second)
    {
      rowsWithActiveColumnSelected->clear();
      lastDirectlySelectedRow = NULL;
    }
  }
  

  void CnetTableViewContent::updateColumnGroupSelection(AbstractTreeItem * item)
  {
    // delete current row selection
    foreach(AbstractTreeItem * row, *lastShiftSelection)
    {
      if (rowsWithActiveColumnSelected->indexOf(row) != -1)
        rowsWithActiveColumnSelected->removeOne(row);
    }

    
    if (lastDirectlySelectedRow)
    {
      *lastShiftSelection = model->getItems(
          lastDirectlySelectedRow, item);
    }
    else
    {
      lastShiftSelection->clear();
    }

    foreach(AbstractTreeItem * row, *lastShiftSelection)
    {
      if (rowsWithActiveColumnSelected->indexOf(row) == -1)
        rowsWithActiveColumnSelected->append(row);
    }
  }


  void CnetTableViewContent::updateRowGroupSelection(int lastRow)
  {
    foreach(AbstractTreeItem * row, *lastShiftSelection)
    {
      row->setSelected(false);
    }

    if (lastDirectlySelectedRow)
    {
      *lastShiftSelection = model->getItems(
          lastDirectlySelectedRow, items->at(lastRow));
    }
    else
    {
      lastShiftSelection->clear();
    }

    foreach(AbstractTreeItem * row, *lastShiftSelection)
    {
      row->setSelected(true);
    }
  }


  void CnetTableViewContent::copySelection() {
    copyCellSelection(false);
  }


  void CnetTableViewContent::copyAll() {
    copyCellSelection(true);
  }


  void CnetTableViewContent::deleteSelectedRows()
  {
    // Prompt the user for confirmation before deletion.
    QMessageBox::StandardButton status = QMessageBox::warning(
        this, "Delete row(s)?", "Delete selected row(s)?",
        QMessageBox::Yes | QMessageBox::No);

    if (status == QMessageBox::Yes)
    {
      // TODO should we store off the selected rows for efficiency?
      QList<AbstractTreeItem *> selectedRows = model->getSelectedItems();

      emit rebuildModels(selectedRows);
      emit modelDataChanged();

      lastShiftSelection->clear();
    }
  }


  void CnetTableViewContent::updateItemList()
  {
    ASSERT(items);

    if (model)
    {
      int startRow = verticalScrollBar()->value();
      int rowCount = (int) ceil(viewport()->height() / (double) rowHeight);
      *items = model->getItems(startRow, startRow + rowCount);

      viewport()->update();
    }
  }
  
  
  void CnetTableViewContent::showContextMenu(QPoint mouseLocation)
  {
    QMenu contextMenu(this);

    // If there is a row selection, show a context menu if the user clicked
    // anywhere on any of the selected row(s).
    if (hasRowSelection() && isMouseInRowSelection(mouseLocation))
    {
      contextMenu.addAction(deleteSelectedRowsAct);
      contextMenu.exec(mapToGlobal(mouseLocation));
    }
    // Only show the context menu for cells if the user right-clicked on the
    // active cell.
    else if (hasActiveCell() && mouseInCellSelection(mouseLocation))
    {
      if (rowsWithActiveColumnSelected->size() > 1)
        contextMenu.addAction(applyToSelectionAct);
      contextMenu.addAction(applyToAllAct);
      contextMenu.exec(mapToGlobal(mouseLocation));
    }
  }
}

