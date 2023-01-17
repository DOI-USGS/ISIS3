/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "TreeViewContent.h"

#include <cmath>
#include <iostream>

#include <QAction>
#include <QLabel>
#include <QMutex>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QSize>
#include <QtCore/qtextstream.h>
#include <QVariant>
#include <QVBoxLayout>

#include "IException.h"
#include "IString.h"

#include "AbstractTreeItem.h"
#include "TableColumn.h"
#include "AbstractTreeModel.h"


namespace Isis {
  TreeViewContent::TreeViewContent(QWidget *parent) :
    QAbstractScrollArea(parent) {
    nullify();

    m_parentView = (TreeView *) parent;

    m_items = new QList< AbstractTreeItem * >;
    m_mousePressPos = new QPoint;
    m_pressedItem = new QPair< AbstractTreeItem *, bool >(NULL, false);
    m_hoveredItem = new QPair< AbstractTreeItem *, bool >(NULL, false);
    m_lastShiftSelection = new QList<AbstractTreeItem *>;

    verticalScrollBar()->setSingleStep(1);
    horizontalScrollBar()->setSingleStep(10);
    m_rowHeight = QFontMetrics(font()).height() + ITEM_PADDING;
    m_contentWidth = 0;

    setMouseTracking(true);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *alternateRowsAct = new QAction("&Alternate row colors", this);
    alternateRowsAct->setCheckable(true);
    connect(alternateRowsAct, SIGNAL(toggled(bool)),
        this, SLOT(setAlternatingRowColors(bool)));
    addAction(alternateRowsAct);
    alternateRowsAct->setChecked(true);
  }


  TreeViewContent::~TreeViewContent() {
    delete m_items;
    m_items = NULL;

    delete m_mousePressPos;
    m_mousePressPos = NULL;

    delete m_pressedItem;
    m_pressedItem = NULL;

    delete m_hoveredItem;
    m_hoveredItem = NULL;

    delete m_lastShiftSelection;
    m_lastShiftSelection = NULL;
  }


  QSize TreeViewContent::minimumSizeHint() const {
    return QWidget::minimumSizeHint();
  }


  QSize TreeViewContent::sizeHint() const {
    return minimumSizeHint();
  }


  AbstractTreeModel *TreeViewContent::getModel() {
    return m_model;
  }


  void TreeViewContent::setModel(AbstractTreeModel *someModel) {
    if (!someModel) {
      IString msg = "Attempted to set a NULL model!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_model) {
      disconnect(m_model, SIGNAL(modelModified()), this, SLOT(refresh()));
      disconnect(m_model, SIGNAL(filterProgressChanged(int)),
          this, SLOT(updateItemList()));
      disconnect(this,
          SIGNAL(treeSelectionChanged(QList< AbstractTreeItem * >)),
          m_model,
          SIGNAL(treeSelectionChanged(QList< AbstractTreeItem * >)));
      disconnect(m_model, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem *>)),
          this, SLOT(scrollTo(QList<AbstractTreeItem *>)));
    }

    m_model = someModel;
    connect(m_model, SIGNAL(modelModified()), this, SLOT(refresh()));
    connect(m_model, SIGNAL(filterProgressChanged(int)),
        this, SLOT(updateItemList()));
    connect(this, SIGNAL(treeSelectionChanged(QList< AbstractTreeItem * >)),
        m_model, SIGNAL(treeSelectionChanged(QList< AbstractTreeItem * >)));
    connect(m_model, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem *>)),
        this, SLOT(scrollTo(QList<AbstractTreeItem *>)));

    refresh();
  }


  void TreeViewContent::refresh() {

    if (m_model) {
      if (!m_model->isFiltering()) {
        QSize modelVisibleSize =
          m_model->getVisibleSize(ITEM_INDENTATION);
        int rowCount = modelVisibleSize.height();
        m_contentWidth = modelVisibleSize.width() + ITEM_INDENTATION;
        verticalScrollBar()->setRange(0, qMax(rowCount - 1, 0));
        horizontalScrollBar()->setRange(0, m_contentWidth - viewport()->width()
            + horizontalScrollBar()->singleStep());
      }

      updateItemList();
      viewport()->update();
    }
  }


  bool TreeViewContent::eventFilter(QObject *target, QEvent *event) {
    return QObject::eventFilter(target, event);
  }


  void TreeViewContent::mouseDoubleClickEvent(QMouseEvent *event) {
    QPoint pressPos = event->pos();
    int index = pressPos.y() / m_rowHeight;

    if (index < m_items->size()) {
      AbstractTreeItem *item = (*m_items)[index];
      item->setExpanded(!item->isExpanded());
      refresh();
    }
  }

  void TreeViewContent::mousePressEvent(QMouseEvent *event) {
    QPoint pressPos = event->pos();
    int index = pressPos.y() / m_rowHeight;

    m_pressedItem->first = NULL;
    m_pressedItem->second = false;

    if (index < m_items->size()) {
      AbstractTreeItem *item = (*m_items)[index];
      if (item->isSelectable() ||
          (item->getFirstVisibleChild() &&
              getArrowRect(item).contains(pressPos))) {
        m_pressedItem->first = item;

        if (item->getFirstVisibleChild()) {
          QRect arrowRect(getArrowRect(item));
          m_pressedItem->second = arrowRect.contains(pressPos);
        }

        QList< AbstractTreeItem * > newlySelectedItems;
        if (!m_pressedItem->second) {
          if (event->modifiers() & Qt::ControlModifier) {
            foreach (AbstractTreeItem * child, item->getChildren()) {
              child->setSelected(!item->isSelected());
              if (child->isSelected())
                newlySelectedItems.append(child);
            }

            item->setSelected(!item->isSelected());
            if (item->isSelected())
              newlySelectedItems.append(item);

            m_lastDirectlySelectedItem = item;
            m_lastShiftSelection->clear();
          }
          else {
            if (event->modifiers() & Qt::ShiftModifier) {
              foreach (AbstractTreeItem * i, *m_lastShiftSelection)
              i->setSelected(false);

              if (m_lastDirectlySelectedItem) {
                // gets the new shift selection without selecting children
                QList< AbstractTreeItem * > tmp =
                  m_model->getItems(m_lastDirectlySelectedItem, item);

                // use tmp to create a new m_lastShiftSelection with children
                // selected as well
                foreach (AbstractTreeItem * i, tmp) {
                  m_lastShiftSelection->append(i);

                  // if this item is a point item then select its children
                  if (i->getPointerType() == AbstractTreeItem::Point) {
                    foreach (AbstractTreeItem * child, i->getChildren()) {
                      child->setSelected(true);
                      m_lastShiftSelection->append(child);
                    }
                  }
                }
              }
              else {
                m_lastShiftSelection->clear();
              }

              foreach (AbstractTreeItem * i, *m_lastShiftSelection) {
                i->setSelected(true);
                newlySelectedItems.append(i);
              }
            }
            else {
              m_model->setGlobalSelection(false);
              item->setSelected(true);
              newlySelectedItems.append(item);
              m_lastDirectlySelectedItem = item;

              if (item->getPointerType() == AbstractTreeItem::Point) {
                foreach (AbstractTreeItem * child, item->getChildren()) {
                  child->setSelected(true);
                  newlySelectedItems.append(child);
                }
              }

              m_lastShiftSelection->clear();
            }
          }

          emit treeSelectionChanged(newlySelectedItems);
        }
      }
    }
    else {
      m_model->setGlobalSelection(false);
    }

    viewport()->update();
  }


  void TreeViewContent::mouseReleaseEvent(QMouseEvent *event) {
    AbstractTreeItem *item = m_pressedItem->first;
    if (item && getArrowRect(item).contains(event->pos())) {
      item->setExpanded(!item->isExpanded());
      refresh();
    }

    m_pressedItem->first = NULL;
    m_pressedItem->second = false;
    viewport()->update();

    QWidget::mousePressEvent(event);
  }


  void TreeViewContent::mouseMoveEvent(QMouseEvent *event) {
    QPoint cursorPos = event->pos();
    int index = cursorPos.y() / m_rowHeight;

    m_hoveredItem->first = NULL;
    m_hoveredItem->second = false;

    if (index < m_items->size() && index >= 0) {
      AbstractTreeItem *item = (*m_items)[index];
      if (item->isSelectable() ||
          (item->getFirstVisibleChild() &&
              getArrowRect(item).contains(cursorPos))) {
        m_hoveredItem->first = item;

        if (item->getFirstVisibleChild()) {
          QRect arrowRect = getArrowRect(item);
          m_hoveredItem->second = arrowRect.contains(cursorPos);
        }
      }
    }

    viewport()->update();
  }


  void TreeViewContent::leaveEvent(QEvent *event) {
    m_hoveredItem->first = NULL;
    m_hoveredItem->second = false;
    viewport()->update();
  }


  void TreeViewContent::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_A &&
        event->modifiers() == Qt::ControlModifier) {
      m_model->setGlobalSelection(true);
      viewport()->update();
      emit treeSelectionChanged();
    }
    else {
      QWidget::keyPressEvent(event);
    }
  }


  void TreeViewContent::paintEvent(QPaintEvent *event) {
    if (m_model) {
      int startRow = verticalScrollBar()->value();
      int rowCount = (int) ceil(viewport()->height() / (double) m_rowHeight);

      QPainter painter(viewport());
      painter.setRenderHints(QPainter::Antialiasing |
          QPainter::TextAntialiasing);

      for (int i = 0; i < rowCount; i++) {
        // Assume the background color should be the base.  Then set odd rows
        // to be the alternate row color if m_alternatingRowColors is set to
        // true.
        QColor backgroundColor = palette().base().color();

        if (i < m_items->size()) {
          if (m_alternatingRowColors && (startRow + i) % 2 == 1)
            backgroundColor = palette().alternateBase().color();

          if (m_items->at(i)->isSelected())
            backgroundColor = palette().highlight().color();
        }

        // define the top left corner of the row and also how big the row is
        QPoint relativeTopLeft(0, i * m_rowHeight);
        QPoint scrollBarPos(horizontalScrollBar()->value(),
            verticalScrollBar()->value());
        QPoint absoluteTopLeft(relativeTopLeft + scrollBarPos);
        QSize rowSize(viewport()->width(), (int) m_rowHeight);

        // Fill in the background with the background color
        painter.fillRect(QRect(relativeTopLeft, rowSize), backgroundColor);

        // if the mouse is hovering over this item, then also draw a rect
        // around this item.
        if (i < m_items->size() && m_hoveredItem->first == (*m_items)[i] &&
            m_hoveredItem->first->isSelectable()) {
          QPen prevPen(painter.pen());
          QPen borderPen(prevPen);
          borderPen.setWidth(1);
          borderPen.setColor(palette().highlight().color());
          painter.setPen(borderPen);
          QPoint borderTopLeft(relativeTopLeft.x() - absoluteTopLeft.x(),
              relativeTopLeft.y() + 1);

          int rectWidth = qMax(m_contentWidth +
              horizontalScrollBar()->singleStep(), viewport()->width());
          QSize borderSize(rectWidth, rowSize.height() - 2);
          painter.drawRect(QRect(borderTopLeft, borderSize));
          painter.setPen(prevPen);
        }

        // if this row has text then draw it
        if (i < m_items->size())
          paintItemText(&painter, i, absoluteTopLeft, relativeTopLeft);
      }
    }
    else {
      QWidget::paintEvent(event);
    }
  }


  void TreeViewContent::resizeEvent(QResizeEvent *event) {
    QAbstractScrollArea::resizeEvent(event);
    horizontalScrollBar()->setRange(0, m_contentWidth - viewport()->width()
        + horizontalScrollBar()->singleStep());
    updateItemList();
  }


  void TreeViewContent::scrollContentsBy(int dx, int dy) {
    QAbstractScrollArea::scrollContentsBy(dx, dy);
    updateItemList();
  }


  void TreeViewContent::nullify() {
    m_parentView = NULL;
    m_model = NULL;
    m_items = NULL;
    m_pressedItem = NULL;
    m_hoveredItem = NULL;
    m_lastDirectlySelectedItem = NULL;
    m_lastShiftSelection = NULL;
    m_mousePressPos = NULL;
  }


  void TreeViewContent::paintItemText(QPainter *painter,
      int index, QPoint absolutePosition, QPoint relativePosition) {
        
    QPoint point(-absolutePosition.x(), relativePosition.y());

    AbstractTreeItem *item = (*m_items)[index];

    // should always be true, but prevents segfault in case of bug
    if (item) {
      // the parameter called point is given to us as the top left corner of
      // the row where the text should go.  We adjust this point until it can
      // be used to draw the text in the middle of the row.  First the x
      // component is adjusted.  How far the x component needs to be adjusted
      // is directly related to how many parents this item has, hence the
      // following while loop.  Note that even top level items have a parent
      // (the invisible root item).  Also note that top level items do not get
      // any adjustment from this while.  This is because all items need
      // exactly one adjustment in the x direction after the arrow is
      // potentially drawn.
      AbstractTreeItem *iteratorItem = item;
      while (iteratorItem->parent() && iteratorItem->parent()->parent()) {
        point.setX(point.x() + ITEM_INDENTATION);
        iteratorItem = iteratorItem->parent();
      }

      QPen originalPen = painter->pen();
      if (item->isSelected()) {
        painter->setPen(QPen(palette().highlightedText().color()));
      }

      // now that the x component has all but its last adjustment taken care
      // of, we then consider items with children.  These items need to have
      // an arrow drawn next to them, before the text is drawn
      if (item->getFirstVisibleChild()) {
        // if the user is hovering over the arrow with the mouse, then draw
        // a box around where the arrow will be drawn
        QRect itemArrowRect(getArrowRect(item));
        if (item == m_hoveredItem->first && item == m_pressedItem->first) {
          if (m_pressedItem->second && m_hoveredItem->second) {
            QPainter::CompositionMode prevMode = painter->compositionMode();
            painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            QColor color = palette().button().color().darker(160);
            color.setAlpha(100);
            painter->fillRect(itemArrowRect, color);
            painter->setCompositionMode(prevMode);
          }
        }

        // if the user has pressed the mouse over the arrow but has not yet
        // released it, then darken the background behind it
        if ((item == m_hoveredItem->first && m_hoveredItem->second) ||
            (item == m_pressedItem->first && m_pressedItem->second)) {
          if (!m_pressedItem->first ||
              (item == m_pressedItem->first && m_pressedItem->second)) {
            painter->drawRect(itemArrowRect);
          }
        }

        // draw the appropriate arrow based on the items expandedness
        if (item->isExpanded())
          drawExpandedArrow(painter, itemArrowRect);
        else
          drawCollapsedArrow(painter, itemArrowRect);
      }

      // the final x component adjustment is the same whether an arrow was
      // drawn or not
      point.setX(point.x() + ITEM_INDENTATION);

      // adjust the y component to center the text vertically in the row
      point.setY(point.y() + ITEM_PADDING / 2);

      // finally draw the text
      int textHeight = m_rowHeight - ITEM_PADDING;
      QRect rect(point, QSize(viewport()->width() - point.x(), textHeight));
      painter->drawText(rect, Qt::TextDontClip, item->getData().toString());
      painter->setPen(originalPen);
    }
  }


  void TreeViewContent::drawCollapsedArrow(QPainter *painter, QRect rect) {
    rect.setTopLeft(rect.topLeft() + QPoint(4, 3));
    rect.setBottomRight(rect.bottomRight() - QPoint(4, 2));

    QPoint top(rect.topLeft());
    QPoint bottom(rect.bottomLeft());
    QPoint right(rect.right(), rect.center().y());

    QPen prevPen = painter->pen();
    QPen arrowPen(prevPen);
    arrowPen.setCapStyle(Qt::RoundCap);
    arrowPen.setJoinStyle(Qt::RoundJoin);
    arrowPen.setWidth(2);
    painter->setPen(arrowPen);
    painter->drawLine(top, right);
    painter->drawLine(bottom, right);
    painter->setPen(prevPen);
  }


  void TreeViewContent::drawExpandedArrow(QPainter *painter, QRect rect) {
    rect.setTopLeft(rect.topLeft() + QPoint(3, 4));
    rect.setBottomRight(rect.bottomRight() - QPoint(2, 4));

    QPoint left(rect.topLeft());
    QPoint right(rect.topRight());
    QPoint bottom(rect.center().x(), rect.bottom());

    QPen prevPen = painter->pen();
    QPen arrowPen(prevPen);
    arrowPen.setCapStyle(Qt::RoundCap);
    arrowPen.setJoinStyle(Qt::RoundJoin);
    arrowPen.setWidth(2);
    painter->setPen(arrowPen);
    painter->drawLine(left, bottom);
    painter->drawLine(right, bottom);
    painter->setPen(prevPen);
  }


  void TreeViewContent::setAlternatingRowColors(bool newStatus) {
    m_alternatingRowColors = newStatus;
    viewport()->update();
  }


  void TreeViewContent::updateItemList() {
    int startRow = verticalScrollBar()->value();
    int rowCount = (int) ceil(viewport()->height() / (double) m_rowHeight);
    *m_items = m_model->getItems(startRow, startRow + rowCount,
        AbstractTreeModel::AllItems, false);

    viewport()->update();
  }


  QRect TreeViewContent::getArrowRect(AbstractTreeItem *item) const {
    QRect arrowRect;
    if (item) {
      int index = m_items->indexOf(item);
      QPoint centerOfArrow(12 - horizontalScrollBar()->value(),
          (index * m_rowHeight) + (m_rowHeight / 2));
      int depth = item->getDepth() - 1;
      centerOfArrow.setX(centerOfArrow.x() + (depth * ITEM_INDENTATION));

      arrowRect = QRect(centerOfArrow.x() - 6, centerOfArrow.y() - 6, 12, 12);
    }

    return arrowRect;
  }

  void TreeViewContent::scrollTo(
    QList< AbstractTreeItem * > newlySelectedItems) {
    if (newlySelectedItems.size())
      scrollTo(newlySelectedItems.last());
  }


  void TreeViewContent::scrollTo(AbstractTreeItem *newlySelectedItem) {
    if (newlySelectedItem->getPointerType() == AbstractTreeItem::Measure)
      newlySelectedItem->parent()->setExpanded(true);

    int row = getModel()->indexOfVisibleItem(newlySelectedItem);

    if (row >= 0) {
      int topRow = verticalScrollBar()->value();

      if (row < topRow) {
        verticalScrollBar()->setValue(row);
      }
      else {
        int wholeVisibleRowCount = viewport()->height() / m_rowHeight;
        int bottomRow = topRow + wholeVisibleRowCount;
        if (row > bottomRow)
          verticalScrollBar()->setValue(row - wholeVisibleRowCount + 1);
      }
    }

    viewport()->update();
  }
}
