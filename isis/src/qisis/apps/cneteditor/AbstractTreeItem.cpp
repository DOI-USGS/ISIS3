#include "IsisDebug.h"

#include "AbstractTreeItem.h"

#include <iostream>

#include <QDateTime>
#include <QFontMetrics>
#include <QLocale>
#include <QVariant>

#include "iException.h"
#include "iString.h"
#include "SpecialPixel.h"

#include "AbstractCnetTableModel.h"
#include "CnetTableColumn.h"


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


  bool AbstractTreeItem::operator<(AbstractTreeItem const & other)
  {
    // first get column
    CnetTableColumn * col = NULL;
    
    if (getPointerType() != other.getPointerType())
    {
      iString msg = "Tried to compare apples to oranges";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    
    QString data = getData(col->getTitle());
    QString otherData = other.getData(col->getTitle());
    
    QString format = "yyyy-MM-ddTHH:mm:ss";
    QDateTime dateTime = QDateTime::fromString(data, format);
    QDateTime otherDateTime = QDateTime::fromString(otherData, format);
    if (dateTime.isValid() && otherDateTime.isValid())
      return dateTime < otherDateTime;
    
    bool doubleDataOk, otherDoubleDataOk;
    double doubleData = data.toDouble(&doubleDataOk);
    double otherDoubleData = otherData.toDouble(&otherDoubleDataOk);
    if (doubleDataOk && otherDoubleDataOk)
      return doubleData < otherDoubleData;
    
    if (doubleDataOk || otherDoubleDataOk)
      return data.toLower() == "null";
    
    return data < otherData;
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
    {
      QLocale locale;
      str = locale.toString(d, 'f');
    }

    return str;
  }


  double AbstractTreeItem::catchNull(QString str)
  {
    double d = Null;
    if (str.toLower() != "null")
    {
      QLocale locale;
      d = locale.toDouble(str);
    }

    return d;
  }
}

