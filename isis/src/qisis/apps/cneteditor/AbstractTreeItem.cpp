#include "IsisDebug.h"

#include "AbstractTreeItem.h"

#include <iostream>

#include <QFontMetrics>
#include <QVariant>

#include "iException.h"
#include "iString.h"
#include "SpecialPixel.h"

namespace Isis
{
  AbstractTreeItem::AbstractTreeItem(AbstractTreeItem * parent) : parentItem(
      parent)
  {
    expanded = false;
    selectable = true;
    selected = false;
    visible = true;
    nextVisibleItem = NULL;
    dataWidth = 0;
  }


  AbstractTreeItem::~AbstractTreeItem()
  {
    nextVisibleItem = NULL;
    parentItem = NULL;
  }


  AbstractTreeItem * AbstractTreeItem::parent() const
  {
    return parentItem;
  }


  void AbstractTreeItem::setParent(AbstractTreeItem * newParent)
  {
    parentItem = newParent;
  }


  int AbstractTreeItem::row() const
  {
    int rowIndex = -1;

    if (parentItem)
      rowIndex = parentItem->indexOf(const_cast< AbstractTreeItem * >(this));

    return rowIndex;
  }


  AbstractTreeItem * AbstractTreeItem::getNextVisiblePeer() const
  {
    return nextVisibleItem;
  }


  void AbstractTreeItem::setNextVisiblePeer(AbstractTreeItem * next)
  {
    nextVisibleItem = next;
  }


  bool AbstractTreeItem::hasPoint(ControlPoint * point) const
  {
    bool found = false;

    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasPoint(point);

    return found;
  }


  bool AbstractTreeItem::hasMeasure(ControlMeasure * measure) const
  {
    bool found = false;

    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasMeasure(measure);

    return found;
  }


  bool AbstractTreeItem::hasNode(ControlCubeGraphNode * cube) const
  {
    bool found = false;

    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasNode(cube);

    return found;
  }


  void AbstractTreeItem::setExpanded(bool newState)
  {
    expanded = newState;
  }

  bool AbstractTreeItem::isExpanded() const
  {
    return expanded;
  }


  void AbstractTreeItem::setSelected(bool newState)
  {
    selected = newState;
  }


  void AbstractTreeItem::setSelectable(bool newSelectable)
  {
    selectable = newSelectable;
  }


  bool AbstractTreeItem::isSelected() const
  {
    return selected;
  }


  bool AbstractTreeItem::isSelectable() const
  {
    return selectable;
  }


  void AbstractTreeItem::setVisible(bool newState)
  {
    visible = newState;
  }


  bool AbstractTreeItem::isVisible() const
  {
    return visible;
  }


  int AbstractTreeItem::getDataWidth() const
  {
    if (dataWidth == 0)
    {
      iString msg = "Children of AbstractTreeItem must call setDataWidth with "
          "a non-zero width";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return dataWidth;
  }


  int AbstractTreeItem::getDepth() const
  {
    int depth = 0;

    AbstractTreeItem * item = parent();

    while (item)
    {
      depth++;
      item = item->parent();
    }

    return depth;
  }


  void AbstractTreeItem::setLastVisibleFilteredItem(AbstractTreeItem * item)
  {
    iString msg = "This tree item does not keep track of visible filtered "
        "items";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  const AbstractTreeItem * AbstractTreeItem::getLastVisibleFilteredItem() const
  {
    return NULL;
  }


  void AbstractTreeItem::calcDataWidth(int avgCharWidth)
  {
    if (avgCharWidth <= 0)
    {
      iString msg = "calcDataWidth() expects a positive non-zero value.";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    dataWidth = (avgCharWidth + 1) * getData().size();
  }


  QString AbstractTreeItem::catchNull(double d)
  {
    QString str = "NULL";
    if (d != Null)
      str = QString::number(d);

    return str;
  }


  double AbstractTreeItem::catchNull(QString str)
  {
    double d = Null;
    if (str.toLower() != "null")
      d = str.toDouble();

    return d;
  }
}

