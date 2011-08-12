#include "IsisDebug.h"

#include "RootItem.h"


namespace Isis
{
  RootItem::RootItem() : AbstractTreeItem(NULL), AbstractNullDataItem()
  {
    lastVisibleFilteredItem = NULL;
    setExpanded(true);
  }


  RootItem::~RootItem()
  {
    lastVisibleFilteredItem = NULL;
  }


  void RootItem::setLastVisibleFilteredItem(AbstractTreeItem * item)
  {
    lastVisibleFilteredItem = item;
  }


  const AbstractTreeItem * RootItem::getLastVisibleFilteredItem() const
  {
    return lastVisibleFilteredItem;
  }
}
