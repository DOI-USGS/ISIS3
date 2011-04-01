#include "IsisDebug.h"

#include "ConnectionParentItem.h"

#include "SerialParentItem.h"


namespace Isis
{
  ConnectionParentItem::ConnectionParentItem(ControlCubeGraphNode * node,
      AbstractTreeItem * parent) : AbstractTreeItem(parent),
    AbstractSerialItem(node)
  {
  }


  ConnectionParentItem::~ConnectionParentItem()
  {
  }


  void ConnectionParentItem::addChild(AbstractTreeItem * child)
  {
    // Only SerialParentItems should be children of ConnectionParentItems
    ASSERT(dynamic_cast< SerialParentItem * >(child));
    AbstractParentItem::addChild(child);
  }
}
