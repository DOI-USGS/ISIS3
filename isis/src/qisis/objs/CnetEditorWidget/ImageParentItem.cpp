#include "IsisDebug.h"

#include "ImageParentItem.h"


namespace Isis {
  ImageParentItem::ImageParentItem(ControlCubeGraphNode *node,
      int avgCharWidth, AbstractTreeItem *parent)
    : AbstractTreeItem(parent), AbstractImageItem(node, avgCharWidth) {
  }


  ImageParentItem::~ImageParentItem() {
  }
}
