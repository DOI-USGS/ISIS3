#include "IsisDebug.h"

#include "AbstractTreeItem.h"

#include <QVariant>


namespace Isis
{
  AbstractTreeItem::AbstractTreeItem(AbstractTreeItem * parent) : parentItem(
      parent)
  {
    expanded = false;
    selected = false;
  }


  AbstractTreeItem::~AbstractTreeItem()
  {
    parentItem = NULL;
  }


  AbstractTreeItem * AbstractTreeItem::parent()
  {
    return parentItem;
  }
  
  
  void AbstractTreeItem::setParent(AbstractTreeItem * newParent)
  {
    parentItem = newParent;
  }


  int AbstractTreeItem::row() const
  {
    ASSERT(parentItem);

    int rowIndex = -1;
    if (parentItem)
      rowIndex = parentItem->indexOf(const_cast< AbstractTreeItem * >(this));

    return rowIndex;
  }


  void AbstractTreeItem::setExpanded(bool newState) { expanded = newState; }
  bool AbstractTreeItem::isExpanded() const { return expanded; }


  void AbstractTreeItem::setSelected(bool newState) { selected = newState; }
  bool AbstractTreeItem::isSelected() const { return selected; }
}
