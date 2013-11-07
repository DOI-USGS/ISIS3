#include "IsisDebug.h"

#include "RootItem.h"


namespace Isis {
  namespace CnetViz {
    RootItem::RootItem() : AbstractTreeItem(NULL), AbstractNullDataItem() {
      m_lastVisibleFilteredItem = NULL;
      setExpanded(true);
    }


    RootItem::~RootItem() {
      m_lastVisibleFilteredItem = NULL;
    }


    void RootItem::setLastVisibleFilteredItem(AbstractTreeItem *item) {
      m_lastVisibleFilteredItem = item;
    }


    const AbstractTreeItem *RootItem::getLastVisibleFilteredItem() const {
      return m_lastVisibleFilteredItem;
    }
  }
}
