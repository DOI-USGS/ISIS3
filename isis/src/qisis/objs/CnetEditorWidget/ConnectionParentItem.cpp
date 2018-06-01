#include "IsisDebug.h"

#include "ConnectionParentItem.h"

#include "ImageParentItem.h"


namespace Isis {
  ConnectionParentItem::ConnectionParentItem(QString imageSerial,
      int avgCharWidth, AbstractTreeItem *parent)
    : AbstractTreeItem(parent), AbstractImageItem(imageSerial, avgCharWidth) {
  }


  ConnectionParentItem::~ConnectionParentItem() {
  }


  void ConnectionParentItem::addChild(AbstractTreeItem *child) {
    // Only ImageParentItems should be children of ConnectionParentItems
    ASSERT(dynamic_cast< ImageParentItem * >(child));

    AbstractParentItem::addChild(child);
  }
}
