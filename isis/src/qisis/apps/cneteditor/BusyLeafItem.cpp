#include "BusyLeafItem.h"

#include <QString>

#include "iException.h"
#include "iString.h"

#include "AbstractTreeItem.h"


namespace Isis
{
  BusyLeafItem::BusyLeafItem(AbstractTreeItem * parent)
      : AbstractTreeItem(parent), AbstractNullDataItem()
  {
    calcDataWidth(1);
  }


  BusyLeafItem::~BusyLeafItem()
  {
  }


  QString BusyLeafItem::getData() const
  {
    return QString("Working...");
  }


  bool BusyLeafItem::isSelectable() const
  {
    return false;
  }
}
