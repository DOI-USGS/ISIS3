#include "BusyLeafItem.h"

#include <QVariant>

#include "IException.h"
#include "IString.h"

#include "AbstractTreeItem.h"


namespace Isis {
  BusyLeafItem::BusyLeafItem(AbstractTreeItem *parent)
    : AbstractTreeItem(parent), AbstractNullDataItem() {
    calcDataWidth(1);
  }


  BusyLeafItem::~BusyLeafItem() {
  }


  QVariant BusyLeafItem::getData() const {
    return QVariant("Working...");
  }


  bool BusyLeafItem::isSelectable() const {
    return false;
  }
}
