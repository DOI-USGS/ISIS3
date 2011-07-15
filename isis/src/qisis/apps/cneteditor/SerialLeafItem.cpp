#include "IsisDebug.h"

#include "SerialLeafItem.h"


namespace Isis
{
  SerialLeafItem::SerialLeafItem(ControlCubeGraphNode * node,
      int avgCharWidth, AbstractTreeItem * parent)
    : AbstractTreeItem(parent), AbstractSerialItem(node, avgCharWidth)
  {
  }


  SerialLeafItem::~SerialLeafItem()
  {
  }
}
