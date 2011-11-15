#include "IsisDebug.h"

#include "ConnectionParentItem.h"

#include "ImageParentItem.h"


namespace Isis
{
  namespace CnetViz
  {
    ConnectionParentItem::ConnectionParentItem(ControlCubeGraphNode * node,
        int avgCharWidth, AbstractTreeItem * parent)
        : AbstractTreeItem(parent), AbstractImageItem(node, avgCharWidth)
    {
    }


    ConnectionParentItem::~ConnectionParentItem()
    {
    }


    void ConnectionParentItem::addChild(AbstractTreeItem * child)
    {
      // Only ImageParentItems should be children of ConnectionParentItems
      ASSERT(dynamic_cast< ImageParentItem * >(child));
      
      AbstractParentItem::addChild(child);
    }
  }
}
