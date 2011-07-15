#include "IsisDebug.h"

#include "SerialParentItem.h"


namespace Isis
{
  SerialParentItem::SerialParentItem(ControlCubeGraphNode * node,
      int avgCharWidth, AbstractTreeItem * parent)
    : AbstractTreeItem(parent), AbstractSerialItem(node, avgCharWidth)
  {
  }


  SerialParentItem::~SerialParentItem()
  {
  }
}
