#include "IsisDebug.h"

#include "SerialParentItem.h"

#include "PointLeafItem.h"


namespace Isis
{
  SerialParentItem::SerialParentItem(ControlCubeGraphNode * node,
      AbstractTreeItem * parent) : AbstractTreeItem(parent),
    AbstractSerialItem(node)
  {
  }


  SerialParentItem::~SerialParentItem()
  {
  }


  void SerialParentItem::addChild(AbstractTreeItem * child)
  {
    // Only PointLeafItems should be children of SerialParentItems
    ASSERT(dynamic_cast< PointLeafItem * >(child));
    AbstractParentItem::addChild(child);
  }
}
