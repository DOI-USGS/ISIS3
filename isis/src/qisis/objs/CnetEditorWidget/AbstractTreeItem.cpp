/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractTreeItem.h"

#include <iostream>

#include <QDateTime>
#include <QFontMetrics>
#include <QLocale>
#include <QVariant>

#include "IException.h"
#include "IString.h"
#include "SpecialPixel.h"

#include "AbstractTableModel.h"
#include "TableColumn.h"


namespace Isis {
  AbstractTreeItem::AbstractTreeItem(AbstractTreeItem *parent) : m_parentItem(
      parent) {
    m_expanded = false;
    m_selectable = true;
    m_selected = false;
    m_visible = true;
    m_nextVisibleItem = NULL;
    m_dataWidth = 0;
  }


  AbstractTreeItem::~AbstractTreeItem() {
    m_nextVisibleItem = NULL;
    m_parentItem = NULL;
  }


  AbstractTreeItem *AbstractTreeItem::parent() const {
    return m_parentItem;
  }


  void AbstractTreeItem::setParent(AbstractTreeItem *newParent) {
    m_parentItem = newParent;
  }


  int AbstractTreeItem::row() const {
    int rowIndex = -1;

    if (m_parentItem)
      rowIndex = m_parentItem->indexOf(const_cast< AbstractTreeItem * >(this));

    return rowIndex;
  }


  QString AbstractTreeItem::getFormattedData() const {
    return catchNull(getData());
  }


  QString AbstractTreeItem::getFormattedData(QString columnTitle) const {
    return catchNull(getData(columnTitle));
  }


  AbstractTreeItem *AbstractTreeItem::getNextVisiblePeer() const {
    return m_nextVisibleItem;
  }


  void AbstractTreeItem::setNextVisiblePeer(AbstractTreeItem *next) {
    m_nextVisibleItem = next;
  }


  bool AbstractTreeItem::hasPoint(ControlPoint *point) const {
    bool found = false;

    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasPoint(point);

    return found;
  }


  bool AbstractTreeItem::hasMeasure(ControlMeasure *measure) const {
    bool found = false;

    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasMeasure(measure);

    return found;
  }


  bool AbstractTreeItem::hasImage(QString imageSerial) const {
    bool found = false;

    for (int i = 0; !found && i < childCount(); i++) {
      found = childAt(i)->hasImage(imageSerial);
    }

    return found;
  }


  void AbstractTreeItem::setExpanded(bool newState) {
    m_expanded = newState;
  }

  bool AbstractTreeItem::isExpanded() const {
    return m_expanded;
  }


  void AbstractTreeItem::setSelected(bool newState) {
    m_selected = newState;
  }


  void AbstractTreeItem::setSelectable(bool newSelectable) {
    m_selectable = newSelectable;
  }


  bool AbstractTreeItem::isSelected() const {
    return m_selected;
  }


  bool AbstractTreeItem::isSelectable() const {
    return m_selectable;
  }


  void AbstractTreeItem::setVisible(bool newState) {
    m_visible = newState;
  }


  bool AbstractTreeItem::isVisible() const {
    return m_visible;
  }


  int AbstractTreeItem::getDataWidth() const {
    if (m_dataWidth == 0) {
      IString msg = "Children of AbstractTreeItem must call setDataWidth "
          "with a non-zero width";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return m_dataWidth;
  }


  int AbstractTreeItem::getDepth() const {
    int depth = 0;

    AbstractTreeItem *item = parent();

    while (item) {
      depth++;
      item = item->parent();
    }

    return depth;
  }


  void AbstractTreeItem::setLastVisibleFilteredItem(AbstractTreeItem *item) {
    IString msg = "This tree item does not keep track of visible filtered "
        "items";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  const AbstractTreeItem *
  AbstractTreeItem::getLastVisibleFilteredItem() const {
    return NULL;
  }


  void AbstractTreeItem::calcDataWidth(int avgCharWidth) {
    if (avgCharWidth <= 0) {
      IString msg = "calcDataWidth() expects a positive non-zero value.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_dataWidth = (avgCharWidth + 1) * getFormattedData().size();
  }


  QString AbstractTreeItem::catchNull(QVariant data) {
    QString result;

    if (data.type() == QVariant::Double) {
      double dblData = data.toDouble();
      result = "NULL";

      if (dblData != Null) {
        QLocale locale;
        result = locale.toString(dblData, 'f');
      }
    }
    else {
      result = data.toString();
    }

    return result;
  }


  double AbstractTreeItem::catchNull(QString str) {
    double d = Null;
    if (str.toLower() != "null") {
      QLocale locale;
      d = locale.toDouble(str);
    }

    return d;
  }
}
