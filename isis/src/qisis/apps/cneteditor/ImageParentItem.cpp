#include "IsisDebug.h"

#include "ImageParentItem.h"


namespace Isis
{
  namespace CnetViz
  {
    ImageParentItem::ImageParentItem(ControlCubeGraphNode * node,
        int avgCharWidth, AbstractTreeItem * parent)
        : AbstractTreeItem(parent), AbstractImageItem(node, avgCharWidth)
    {
    }


    ImageParentItem::~ImageParentItem()
    {
    }
  }
}
